#include "core.hpp"

void Engine::init_engine() {
    m_stop = false;
    rendererVK.init();

    Particle particle;
    particle.curr_pos = vec2(100.f, 100.f);
    particle.radius = 10.f;

    particles.push_back(particle);

    InitializeParticles(20000, 2, particles);
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
    m_input.update(m_event, m_stop);
    if(m_input.isKeyPressed(SDL_SCANCODE_ESCAPE)) {
        m_stop = true;
    }
}

void Engine::update() {
    vec2 mousePos;
    SDL_GetMouseState(&mousePos.x, &mousePos.y);

    particles[0].curr_pos = mousePos;

    //std::cout << mousePos << std::endl;
}

void Engine::render() {
    rendererVK.draw(particles);
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

    ImGui::Text("Physics time: ms | Render Time: ms | FPS: | Particles: ");

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
