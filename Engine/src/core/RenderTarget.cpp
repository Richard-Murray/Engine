#include "RenderTarget.h"

RenderTarget::RenderTarget()
{
	m_width = 1024;
	m_height = 768;
}

RenderTarget::~RenderTarget()
{
	DeleteFrameBuffers();
}

void RenderTarget::SetSize(int width, int height)
{
	m_width = width;
	m_height = height;
}

void RenderTarget::Initialise()
{
	m_attachedColourBuffers.clear();

	// setup and bind a framebuffer
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	// unbind the FBO so that we can render to the back buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::AttachColourBuffer(unsigned int uiAttachmentID, unsigned int uiColourType)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glGenTextures(1, &m_FBOTexture[uiAttachmentID]);
	glBindTexture(GL_TEXTURE_2D, m_FBOTexture[uiAttachmentID]);

	// specify texture format for storage
	glTexStorage2D(GL_TEXTURE_2D, 1, uiColourType, m_width, m_height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// attach it to the framebuffer as the first colour attachment
	// the FBO MUST still be bound
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + uiAttachmentID, m_FBOTexture[uiAttachmentID], 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_attachedColourBuffers.push_back(GL_COLOR_ATTACHMENT0 + uiAttachmentID);
}

void RenderTarget::AttachDepthBuffer(unsigned int uiDepthType, bool bFrameBuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	if (bFrameBuffer == false)
	{
		// setup and bind a 24bit depth buffer as a render buffer
		glGenRenderbuffers(1, &m_FBODepth);
		glBindRenderbuffer(GL_RENDERBUFFER, m_FBODepth);
		glRenderbufferStorage(GL_RENDERBUFFER, uiDepthType, m_width, m_height);

		// while the FBO is still bound
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_FBODepth);

	}
	else
	{
		glGenTextures(1, &m_FBODepth);
		glBindTexture(GL_TEXTURE_2D, m_FBODepth);

		// texture uses a 16-bit depth component format
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// attached as a depth attachment to capture depth not colour
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			m_FBODepth, 0);

	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::SetDrawBuffers()
{
	if (m_attachedColourBuffers.empty())
	{

		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
		glDrawBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
		glDrawBuffers(m_attachedColourBuffers.size(), &m_attachedColourBuffers[0]);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void RenderTarget::SetAsActiveRenderTarget()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glViewport(0, 0, m_width, m_height);
}

void RenderTarget::ClearAsActiveRenderTarget()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);
}

void RenderTarget::DeleteFrameBuffers()
{
	ClearAsActiveRenderTarget();
	glDeleteFramebuffers(1, &m_FBO);
	glDeleteTextures(15, &m_FBOTexture[0]);
	glDeleteRenderbuffers(1, &m_FBODepth);
}