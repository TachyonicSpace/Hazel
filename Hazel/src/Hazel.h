#pragma once

//For use by applications

#include "Hazel\Core\Application.h"
#include "Hazel\Core\Layer.h"
#include "Hazel\Core\log.h"
#include "Hazel\Core\Assert.h"

#include "Hazel\Core\Timestep.h"

#include "Hazel\Core\input.h"
#include "Hazel\Core\MouseButtonCodes.h"
#include "Hazel\Core\KeyCodes.h"

#include "Hazel\ImGui\ImGuiLayer.h"

#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Entity.h"
#include "Hazel/Scene/Components.h"


#include "Hazel/Renderer/Color.h"

#include "Hazel\Renderer\Renderer.h"
#include "Hazel\Renderer\Renderer2D.h"
#include "Hazel\Renderer\RenderCommand.h"
#include "Hazel\Renderer\Framebuffer.h"

#include "Hazel\Renderer\Buffer.h"
#include "Hazel\Renderer\Shader.h"
#include "Hazel\Renderer\Texture.h"
#include "Hazel\Renderer\CameraController.h"
#include "Hazel\Renderer\VertexArray.h"




//------Entry Point--------------------------
#include "Hazel\Core\EntryPoint.h"
//-------------------------------------------