#include "core.hpp"

void Engine::init_engine() {
    m_stop = false;
    rendererVK.init(windowSize.x, windowSize.y);

    Particle particle;
    particle.curr_pos = vec2(100.f, 100.f);
    particle.radius = 10.f;
    particle.isPlayer = true;

    particles.push_back(particle);

    InitializeParticles(m_engineData.spawnParticles, m_engineData.particleRadius, particles);
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
        particles[0].radius = 20.f;
        particles[0].curr_pos = mousePos;
    } else {
        particles[0].radius = 0.0f;
        particles[0].curr_pos = mousePos;
    }

    if(perfData.frameTime < 16.666f) {
        if (m_input.isMouseButtonPressed(SDL_BUTTON_RIGHT)) {
            constexpr float PI = 3.14159f;
            const float spawnRadius = 10.f;
            const int numParticlesToSpawn = 30;

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
                newParticle.isPlayer = false;
                
                particles.push_back(newParticle);
            }
        }
    }
}

void Engine::update() {
    auto start = std::chrono::high_resolution_clock::now();

    int numSteps = 0;
    const int maxCatchUpSteps = 6;

    int width, height;
    SDL_GetWindowSizeInPixels(rendererVK._window, &width, &height);
    windowSize = vec2(width, height);
    physics.setPhysicsBoundary(vec2(0.0, 0.0), windowSize);

    while(m_engineData.accumulator >= m_engineData.timeStep && numSteps < maxCatchUpSteps) {
        const float sub_dt = m_engineData.timeStep / float(m_engineData.maxSteps);
        for(int i = 0; i < m_engineData.maxSteps; ++i) {
            physics.update(particles, sub_dt);
        }
        m_engineData.accumulator -= m_engineData.timeStep;
        numSteps++;
    }

    if(numSteps >= maxCatchUpSteps) {
        m_engineData.accumulator = 0;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto updateDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    perfData.updateTime = updateDuration.count();
}

void Engine::render() {
    auto start = std::chrono::high_resolution_clock::now();

    rendererVK.draw(particles);

    auto end = std::chrono::high_resolution_clock::now();
    auto renderDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    perfData.renderTime = renderDuration.count();

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

    ImGui::Text("Update time: %.2f ms | Render Time: %.2f ms | FPS: %2.f | Particles: %d", perfData.updateTime, perfData.renderTime, perfData.fps, particles.size());

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
