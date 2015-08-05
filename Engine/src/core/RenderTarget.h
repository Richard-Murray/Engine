#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>

class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();

	void Initialise();

	void SetSize(int width, int height);

	//Sets this target to be drawn to
	void SetAsActiveRenderTarget();

	//Resets the frame buffer to the back buffer (can be called from any render target)
	void ClearAsActiveRenderTarget();

	void AttachColourBuffer(unsigned int uiAttachmentID, unsigned int uiColourType);
	void AttachDepthBuffer(unsigned int uiDepthType = GL_DEPTH_COMPONENT24, bool bFrameBuffer = false);

	void SetDrawBuffers();

	void DeleteFrameBuffers();

	unsigned int GetFBO() const { return m_FBO; }
	unsigned int GetRenderTexture(int uiAttachmentID) const { return m_FBOTexture[uiAttachmentID]; }
	unsigned int GetDepthTexture() const { return m_FBODepth; }

private:
	unsigned int m_FBO;
	unsigned int m_FBOTexture[15];
	unsigned int m_FBODepth;

	unsigned int m_width;
	unsigned int m_height;

	std::vector<unsigned int> m_attachedColourBuffers;
};

#endif