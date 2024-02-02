#include "huffman.h"
#include "SimpleHuffmanApp.h"
#include "imgui.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
namespace SimpleHuffmanApp
{
    static ImGuiTextBuffer log_com;
    unsigned char buffer_serial[1024];
    char Ret_codes[200]="";

    int initial_msg = 1234;

    void huffman_main()
    {
        char arr[] = {'a', 'b', 'c', 'd', 'e', 'f'};
        int freq[] = {5, 9, 12, 13, 16, 45};
        int size = sizeof(arr) / sizeof(arr[0]);
        printf("Caracterele si frecventa lor:\n");
        log_com.appendf("Chars frequency: \n");
        for (int i = 0; i < size; ++i)
        {
            printf("%c: %d\n", arr[i], freq[i]);
            log_com.appendf("%c: %d \n", arr[i], freq[i]);
        }
        char * codes = HuffmanCodesS(arr, freq, size);
        log_com.appendf("Hoffman codes: %s \n", codes);
        printf("\nCodurile Huffman: %s\n", codes);
    }

    void RenderUi(void)
    {

        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", nullptr, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
        
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                ImGui::MenuItem("Padding", NULL, &opt_padding);
                ImGui::Separator();

                if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0))
                {
                    dockspace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode;
                }
                if (ImGui::MenuItem("Flag: NoDockingSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0))
                {
                    dockspace_flags ^= ImGuiDockNodeFlags_NoDockingSplit;
                }
                if (ImGui::MenuItem("Flag: NoUndocking", "", (dockspace_flags & ImGuiDockNodeFlags_NoUndocking) != 0))
                {
                    dockspace_flags ^= ImGuiDockNodeFlags_NoUndocking;
                }
                if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))
                {
                    dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
                }
                if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))
                {
                    dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
                }
                if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen))
                {
                    dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
                }
                ImGui::Separator();

                // if (ImGui::MenuItem("Close", NULL, false))
                //     *p_open = false;
                ImGui::EndMenu();
            }


            ImGui::EndMenuBar();
        }
        ImGui::Begin("Settings");
        // ImGui::InputInt("Input prime number (p)", &prime_p);
        // ImGui::InputInt("Input prime number (q)", &prime_q);
        // ImGui::InputInt("Input number to be encrypted", &initial_msg);

        if(ImGui::Button("Compute Huffman"))
        {
            SimpleHuffmanApp::huffman_main();
        }
        ImGui::End();

        ImGui::Begin("Viewport");
        struct Funcs
        {
            static int MyResizeCallback(ImGuiInputTextCallbackData *data)
            {
                if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
                {
                    ImVector<char> *my_str = (ImVector<char> *)data->UserData;
                    IM_ASSERT(my_str->begin() == data->Buf);
                    my_str->resize(data->BufSize); // NB: On resizing calls, generally data->BufSize == data->BufTextLen + 1
                    data->Buf = my_str->begin();
                }
                return 0;
            }
            // Note: Because ImGui:: is a namespace you would typically add your own function into the namespace.
            // For example, you code may declare a function 'ImGui::InputText(const char* label, MyString* my_str)'
            static bool MyInputTextMultiline(const char *label, ImVector<char *> *my_str, const ImVec2 &size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0)
            {
                IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
                return ImGui::InputTextMultiline(label, (char *)my_str->begin(), (size_t)my_str->size(), size, flags | ImGuiInputTextFlags_CallbackResize, Funcs::MyResizeCallback, (void *)my_str);
            }
        };
        static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
         if (log_com.size() > 2024)
             log_com.clear();
        // if ((buffer_serial[0] != 0x00) && (logging_serial_com == true))
        //     log_com.appendf(">> %s", buffer_serial);
       
        ImGui::InputTextMultiline("##MyStr", (char *)log_com.c_str(), log_com.size(), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() *64), flags, Funcs::MyResizeCallback, (void *)buffer_serial);
        ImGui::End();


        ImGui::End();
    }
}