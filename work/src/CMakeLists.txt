#
# CGRA 408
#

SET(headers
	"image.h"
	"camera.h"
	"ray.h"
	"parser.h"
	
	"aShape.h"
	"sphere.h"
	"cube.h"
	"plane.h"
	"cylinder.h"
	"triangle.h"
	"mesh.h"
	"bvh.h"
	"bbox.h"
	
	"aLight.h"
	"pointLight.h"
)

SET(sources
	"main.cpp"
	"camera.cpp"
	"sphere.cpp"
	"cube.cpp"
	"plane.cpp"
	"cylinder.cpp"
	"triangle.cpp"
	"mesh.cpp"
	"bvh.cpp"
	"parser.cpp"

	"pointLight.cpp"
)

# Add executable target and link libraries
# You do not need to touch this
add_executable(${PROJECT_1} ${headers} ${sources})
target_link_libraries(${PROJECT_1} PRIVATE glm)
target_link_libraries(${PROJECT_1} PRIVATE stb)
if (UNIX)
	target_link_libraries(${PROJECT_1} PRIVATE pthread)
endif (UNIX)