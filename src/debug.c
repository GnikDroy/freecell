#include "debug.h"

#ifdef DEBUG
#include "log.h"
void APIENTRY openglDebugCallback(GLenum source, GLenum type, GLuint id,
                                  GLenum severity, GLsizei length,
                                  const GLchar *message,
                                  const void *userParam) {
  (void)length;
  (void)userParam;

  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    return;

  log_error("OpenGL Debug Message:\n"
            "Source: 0x%x\n"
            "Type: 0x%x\n"
            "ID: %u\n"
            "Severity: 0x%x\n"
            "Message: %s\n\n",
            source, type, id, severity, message);
}
#else
void APIENTRY openglDebugCallback(GLenum source, GLenum type, GLuint id,
                                  GLenum severity, GLsizei length,
                                  const GLchar *message,
                                  const void *userParam) {
  (void)source;
  (void)type;
  (void)id;
  (void)severity;
  (void)length;
  (void)message;
  (void)userParam;
}
#endif
