#include "core.hpp"

void Engine::init_engine() {
    m_stop = false;
    rendererVK.init(windowSize.x, windowSize.y);

    particles.reserve(m_engineData.MaxParticles);

    Particle particle;
    particle.curr_pos = vec2(0.f, 0.f);
    particle.radius = 0.f;

    particles.add_particle(particle.curr_pos, particle.prev_pos, particle.radius);
}

void Engine::shutdown() {
    rendererVK.cleanup();
    SDL_Quit();
}

void Engine::run() {
    init_engine();

    m_engineData.previousTicks = SDL_GetPerformanceCounter();
    double performanceFrequency = static_cast<double>(SDL_GetPerformanceFrequency());

    while(!m_stop) {
        m_engineData.currentTicks = SDL_GetPerformanceCounter();
        m_engineData.elapsedTicks = m_engineData.currentTicks - m_engineData.previousTicks;
        double frameTime = m_engineData.elapsedTicks / performanceFrequency;

        if(frameTime > m_engineData.maxFrameTime) {
           frameTime = m_engineData.maxFrameTime;
        }

        m_engineData.accumulator += frameTime;

        input();
        update();

        renderGui();
        render();

        double currentFrameTime = (SDL_GetPerformanceCounter() - m_engineData.currentTicks) / performanceFrequency;
        while(currentFrameTime < m_engineData.targetFrameTime) {
              currentFrameTime = (SDL_GetPerformanceCounter() - m_engineData.currentTicks) / performanceFrequency;
        }
    }

    shutdown();
}

void Engine::input() {
    m_input.update(m_event, m_stop, rendererVK);

    vec2 mousePos;
    
    SDL_GetMouseState(&mousePos.x, &mousePos.y);
    if(m_input.isMouseButtonPressed(SDL_BUTTON_LEFT)) {
        particles.radius[0] = 20.f;
        particles.curr_pos_x[0] = mousePos.x;
        particles.curr_pos_y[0] = mousePos.y;
    } else {
        particles.radius[0] = 0.0f;
        particles.curr_pos_x[0] = mousePos.x;
        particles.curr_pos_y[0] = mousePos.y;
    }


    if(perfData.frameTime < 16.f) {
        if (m_input.isMouseButtonPressed(SDL_BUTTON_RIGHT)) {
            constexpr float PI = 3.14159f;
            const float spawnRadius = 10.f;
            const int numParticlesToSpawn = 32;

            for (int i = 0; i < numParticlesToSpawn; i++) {
                float randomAngle = static_cast<float>(rand()) / RAND_MAX * 2.f * PI;
                float randomDistance = static_cast<float>(rand()) / RAND_MAX * spawnRadius;
                
                vec2 offset = {
                    randomDistance * glm::cos(randomAngle),
                    randomDistance * glm::sin(randomAngle)
                };
                
                vec2 spawnPos = mousePos + offset;

                Particle newParticle;
                newParticle.radius = m_engineData.particleRadius;
                newParticle.curr_pos = spawnPos;
                newParticle.prev_pos = spawnPos;
                
                particles.add_particle(spawnPos, spawnPos, m_engineData.particleRadius);
            }
        }
    }
}

void Engine::update() {
    auto start = std::chrono::high_resolution_clock::now();

    int numSteps = 0;

    int width, height;
    SDL_GetWindowSizeInPixels(rendererVK._window, &width, &height);
    windowSize = vec2(width, height);
    physics.setPhysicsBoundary(vec2(0.0, 0.0), windowSize);

    while(m_engineData.accumulator >= m_engineData.timeStep && numSteps < m_engineData.maxCatchUpSteps) {
        const float sub_dt = m_engineData.timeStep / float(m_engineData.maxSteps);
        for(int i = 0; i < m_engineData.maxSteps; ++i) {
            physics.update(particles, perfData, sub_dt);
        }
        m_engineData.accumulator -= m_engineData.timeStep;
        numSteps++;
    }

    if(numSteps >= m_engineData.maxCatchUpSteps) {
        m_engineData.accumulator = 0;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto updateDuration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    perfData.updateTime = updateDuration.count() / 1000.f;
}

void Engine::render() {
    auto start = std::chrono::high_resolution_clock::now();

    rendererVK.draw(particles);

    auto end = std::chrono::high_resolution_clock::now();
    auto renderDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    perfData.renderTime = renderDuration.count() / 1000.f;

    perfData.frameTime = 0;
    perfData.frameTime = perfData.updateTime + perfData.renderTime;
    perfData.fps = 1000.f / perfData.frameTime;
}

void Engine::renderGui() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("Stats", nullptr, 
        ImGuiWindowFlags_NoDecoration | 
        ImGuiWindowFlags_NoBackground | 
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    ImGui::Text("Update time: %.2f ms | Render time: %.2f ms | FPS: %2.f | Particles: %d", perfData.updateTime, perfData.renderTime, perfData.fps, particles.size());

    ImGui::PopStyleColor();
    ImGui::End();

    if(ImGui::Begin("background")) {
        ComputeEffect& selected = rendererVK.backgroundEffects[rendererVK.currentBackgroundEffect];

        ImGui::Text("Selected Background: ", selected.name);
        ImGui::SliderInt("Background Index", &rendererVK.currentBackgroundEffect, 0, rendererVK.backgroundEffects.size() - 1);

        ImGui::InputFloat4("data1",(float*)& selected.data.data1);
		ImGui::InputFloat4("data2",(float*)& selected.data.data2);
		ImGui::InputFloat4("data3",(float*)& selected.data.data3);
		ImGui::InputFloat4("data4",(float*)& selected.data.data4);
    }

    ImGui::End();
    ImGui::Render();
}
