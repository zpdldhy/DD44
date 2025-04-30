#include "pch.h"
#include "ImGuiCore.h"
#include "Device.h"
#include "UMaterial.h"

void ImGuiCore::Init()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    if (m_bDark == true)
    {
        ImGui::StyleColorsDark();
    }
    else
    {
        ImGui::StyleColorsLight();
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(DEVICE.Get(), DC.Get());
}

void ImGuiCore::Update()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    Test();
}

void ImGuiCore::Render()
{
    // Rendering
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiCore::Test()
{
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);
        ImGui::Checkbox("Show Glow UI", &m_bShowGlowControl);   // UI박스 ON/OFF

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    // Test
    {
        ImGui::Begin("Test", nullptr, // Key값: 하나의 창
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize);

        ImGui::Text("Hello fucking world!");

        ImGui::End();
    }


    //Test2
    if (m_bShowGlowControl)
    {
        ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Once); // 창 크기는 한번만 설정
        ImGui::Begin("Test Glow Control", &m_bShowGlowControl, ImGuiWindowFlags_NoResize);

        ImGui::Text("Target Cube");
        ImGui::RadioButton("Cube 1", &m_iSelectedActor, 0); ImGui::SameLine();
        ImGui::RadioButton("Cube 2", &m_iSelectedActor, 1);
        ImGui::Text("Glow_Test");
        
        float color[3] = { m_vGlowColor.x, m_vGlowColor.y, m_vGlowColor.z };
        if (ImGui::ColorEdit3("Glow Color", color, ImGuiColorEditFlags_Float))
        {
            m_vGlowColor = { color[0], color[1], color[2] };
        }
        ImGui::SliderFloat("Glow Power", &m_fGlowPower, 0.0f, 5.0f);
              

        ImGui::SliderFloat("Dissolve Threshold", &m_fDissolveThreshold, 0.0f, 1.0f);


        ImGui::End();
    }



    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }
}
