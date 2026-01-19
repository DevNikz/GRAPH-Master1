#pragma once

#include <vector>
#include <cstdint>
#include <functional>
#include <mutex>
#include <glm/glm.hpp>

struct MeshHandle {
	uint32_t vao = 0;
	uint32_t vbo = 0;
	uint32_t ebo = 0;
	uint32_t index_count = 0;
};

class GLRenderer {
public:
	GLRenderer();
	~GLRenderer();

	void Init();
	MeshHandle UploadMesh(const std::vector<float>& vertex_positions, const std::vector<uint32_t>& indices);
	void RenderMesh(const MeshHandle& h, const glm::mat4& model, const glm::mat4& viewProj, const glm::vec3& color);
	void DestroyMesh(MeshHandle& h);

private:
	uint32_t CompileShader(uint32_t type, const char* src);
	uint32_t CreateProgram(const char* vs_src, const char* fs_src);

	uint32_t program_ = 0;
};