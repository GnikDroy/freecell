#include "debug.h"
#include <glad/glad.h>

#ifdef DEBUG
 #include <stdio.h>
void APIENTRY openglDebugCallback(GLenum source, GLenum type, GLuint id,
                                  GLenum severity, GLsizei length,
                                  const GLchar *message,
                                  const void *userParam) {
  (void)length;
  (void)userParam;

  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    return;

  fprintf(stderr, "OpenGL Debug Message:\n");
  fprintf(stderr, "Source: 0x%x\n", source);
  fprintf(stderr, "Type: 0x%x\n", type);
  fprintf(stderr, "ID: %u\n", id);
  fprintf(stderr, "Severity: 0x%x\n", severity);
  fprintf(stderr, "Message: %s\n\n", message);
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
