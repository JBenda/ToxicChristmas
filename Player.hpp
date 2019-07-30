#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace glm {
	typedef glm::mat4x4 mat4;
};

class Player {
public:
	Player(void ** viewMat) : m_mView(1.0f) {
		*viewMat = glm::value_ptr(m_mView);
		
	}
private:
	glm::vec3 m_pos;
	glm::vec3 m_vel;
	glm::mat4 m_mView;
};
