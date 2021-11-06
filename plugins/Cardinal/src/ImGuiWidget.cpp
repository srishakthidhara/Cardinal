/*
 * Dear ImGui for DPF, converted to VCV
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>
 * Copyright (C) 2021 Jean Pierre Cimalando <jp-dev@inbox.ru>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "ImGuiWidget.hpp"
#include "DistrhoUtils.hpp"

#ifndef DGL_NO_SHARED_RESOURCES
# include "../../../dpf/dgl/src/Resources.hpp"
#endif

#include "DearImGui/imgui_impl_opengl2.h"

struct ImGuiWidget::PrivateData {
    ImGuiContext* context = nullptr;
    bool created = false;

    PrivateData(const double scaleFactor = 1.0)
    {
        IMGUI_CHECKVERSION();
        context = ImGui::CreateContext();
        ImGui::SetCurrentContext(context);

        ImGuiIO& io(ImGui::GetIO());
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.IniFilename = nullptr;

        /*
        ImGuiStyle& style(ImGui::GetStyle());
        style.ScaleAllSizes(scaleFactor);
        */

#ifndef DGL_NO_SHARED_RESOURCES
        using namespace dpf_resources;
        ImFontConfig fc;
        fc.FontDataOwnedByAtlas = false;
        fc.OversampleH = 1;
        fc.OversampleV = 1;
        fc.PixelSnapH = true;
        io.Fonts->AddFontFromMemoryTTF((void*)dejavusans_ttf, dejavusans_ttf_size, 13.0f * scaleFactor, &fc);
        io.Fonts->Build();
#endif

        io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
        io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
        io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
        io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
        io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
        io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
        io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
        io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
        io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
        io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
        io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
        io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
        io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
        io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
        io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
        io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
        io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
        io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;
    }

    ~PrivateData()
    {
        // this should not happen
        if (created)
        {
            ImGui::SetCurrentContext(context);
            ImGui_ImplOpenGL2_Shutdown();
        }

        ImGui::DestroyContext(context);
    }
};

ImGuiWidget::ImGuiWidget()
    : imData(new PrivateData()) {}

ImGuiWidget::~ImGuiWidget()
{
    delete imData;
}

void ImGuiWidget::onContextCreate(const ContextCreateEvent& e)
{
    OpenGlWidget::onContextCreate(e);
    DISTRHO_SAFE_ASSERT_RETURN(!imData->created,);

    ImGui::SetCurrentContext(imData->context);
    ImGui_ImplOpenGL2_Init();
    imData->created = true;
}

void ImGuiWidget::onContextDestroy(const ContextDestroyEvent& e)
{
    if (imData->created)
    {
        ImGui::SetCurrentContext(imData->context);
        ImGui_ImplOpenGL2_Shutdown();
        imData->created = false;
    }

    OpenGlWidget::onContextDestroy(e);
}

void ImGuiWidget::drawFramebuffer()
{
    const math::Vec fbSize = getFramebufferSize();

    ImGui::SetCurrentContext(imData->context);
    ImGuiIO& io(ImGui::GetIO());

    io.DisplayFramebufferScale = ImVec2(fbSize.x / box.size.x, fbSize.y / box.size.y);
    io.DisplaySize = ImVec2(box.size.x, box.size.y);

    if (!imData->created)
    {
        ImGui_ImplOpenGL2_Init();
        imData->created = true;
    }

    // TODO io.DeltaTime

    ImGui_ImplOpenGL2_NewFrame();
    ImGui::NewFrame();

    drawImGui();

    ImGui::Render();

    if (ImDrawData* const data = ImGui::GetDrawData())
        ImGui_ImplOpenGL2_RenderDrawData(data);
}

void ImGuiWidget::onHover(const HoverEvent& e)
{
    ImGui::SetCurrentContext(imData->context);

    ImGuiIO& io(ImGui::GetIO());
    io.MousePos.x = e.pos.x + e.mouseDelta.x;
    io.MousePos.y = e.pos.y + e.mouseDelta.y;
}

void ImGuiWidget::onDragHover(const DragHoverEvent& e)
{
    ImGui::SetCurrentContext(imData->context);

    ImGuiIO& io(ImGui::GetIO());
    io.MousePos.x = e.pos.x + e.mouseDelta.x;
    io.MousePos.y = e.pos.y + e.mouseDelta.y;
}

void ImGuiWidget::onDragLeave(const DragLeaveEvent& e)
{
    ImGui::SetCurrentContext(imData->context);

    // FIXME this is not the correct event..
    ImGuiIO& io(ImGui::GetIO());
    io.MouseDown[0] = io.MouseDown[1] = io.MouseDown[2] = false;

    if (io.WantCaptureMouse)
        e.consume(this);
}

void ImGuiWidget::onHoverScroll(const HoverScrollEvent& e)
{
    ImGui::SetCurrentContext(imData->context);

    ImGuiIO& io(ImGui::GetIO());
    io.MouseWheel += e.scrollDelta.y * 0.01f;
    io.MouseWheelH += e.scrollDelta.x * 0.01f;

    if (io.WantCaptureMouse)
        e.consume(this);
}

void ImGuiWidget::onButton(const ButtonEvent& e)
{
    ImGui::SetCurrentContext(imData->context);

    ImGuiIO& io(ImGui::GetIO());

    switch (e.button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
        io.MouseDown[0] = e.action == GLFW_PRESS;
        break;
    /* Don't capture these, let Cardinal handle it instead
    case GLFW_MOUSE_BUTTON_MIDDLE:
        io.MouseDown[1] = e.action == GLFW_PRESS;
        break;
    case GLFW_MOUSE_BUTTON_RIGHT:
        io.MouseDown[2] = e.action == GLFW_PRESS;
        break;
    */
    default:
        return;
    }

    io.KeyCtrl  = e.mods & GLFW_MOD_CTRL;
    io.KeyShift = e.mods & GLFW_MOD_SHIFT;
    io.KeyAlt   = e.mods & GLFW_MOD_ALT;
    io.KeySuper = e.mods & GLFW_MOD_SUPER;

    if (io.WantCaptureMouse)
        e.consume(this);
}

void ImGuiWidget::onSelectKey(const SelectKeyEvent& e)
{
    if (e.key < 0 || e.key >= IM_ARRAYSIZE(ImGuiIO::KeysDown))
        return;

    ImGui::SetCurrentContext(imData->context);

    ImGuiIO& io(ImGui::GetIO());

    switch (e.action)
    {
    case GLFW_PRESS:
        io.KeysDown[e.key] = true;
        break;
    case GLFW_RELEASE:
        io.KeysDown[e.key] = false;
        break;
    default:
        return;
    }

    io.KeyCtrl  = e.mods & GLFW_MOD_CTRL;
    io.KeyShift = e.mods & GLFW_MOD_SHIFT;
    io.KeyAlt   = e.mods & GLFW_MOD_ALT;
    io.KeySuper = e.mods & GLFW_MOD_SUPER;

    if (io.WantCaptureKeyboard)
        e.consume(this);
}

void ImGuiWidget::onSelectText(const SelectTextEvent& e)
{
    ImGui::SetCurrentContext(imData->context);

    ImGuiIO& io(ImGui::GetIO());
    io.AddInputCharacter(e.codepoint);

    if (io.WantCaptureKeyboard)
        e.consume(this);
}
