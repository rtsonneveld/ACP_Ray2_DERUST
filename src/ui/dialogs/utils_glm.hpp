#include <glm/glm.hpp>

struct Scene;
struct Shader;

glm::vec3* GetGlmPosition();

void DR_DLG_Utils_Init_GLM();
void DR_DLG_Utils_DrawTab_GLM();

void DrawGLM(Scene* scene, Shader* shader);
void HandleGLMUpdates();

static void DrawLine(Shader* shader, glm::vec3 A, glm::vec3 B, unsigned int texture);