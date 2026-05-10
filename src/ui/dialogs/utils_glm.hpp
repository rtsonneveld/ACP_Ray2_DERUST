#include <glm/glm.hpp>

struct Scene;
struct Shader;

glm::vec3* GetGlmPosition();

void DR_DLG_Utils_Init_GLM();
void DR_DLG_Utils_DrawTab_GLM();

void DrawGLM(Scene* scene, Shader* shader);
void HandleGLMUpdates();