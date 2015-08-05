#include "core/gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include <stdio.h>

#include "Application.h"

int main() {

	Application *application = new Application();

	application->Run();
}
