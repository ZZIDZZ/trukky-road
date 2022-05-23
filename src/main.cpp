#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <glew.h>
#include <GLFW/glfw3.h>
#include "game.h"
#include "game.cpp"

using namespace glm;

/* TODO: 
player
load object optimization
collision
 */

float carmov = 0;
bool game = true;
std::vector<Car> car(1, Car());
Player player;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(key == GLFW_KEY_UNKNOWN) return;
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
		if (player.x < 6.5) {
			player.move(1);
			printf("%d\n", player.x);
		}	
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
		if (player.x > -6.5) {
			player.move(-1);
			printf("%d\n", player.x);
		}	
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
		player.jump();
	}
	if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
		if (noclip == true) noclip = false;
		else noclip = true;
	}
}

void youLose() {
	// game = false;
}

void renderCar(glm::mat4 model, GLuint varray_id) {
	glBindVertexArray(varray_id);
	MVP = VP * model;
	glUniform1i(textureID, 2);
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(modelMatID, 1, GL_FALSE, &model[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, car_vertices.size());
}

void handleCar(){
    for(auto it = car.begin(); it != car.end();){
        if ((it -> x != player.x && it -> z < 10) || (it -> x == player.x && ((it -> z < -5.5 && !player.onAir) || player.onAir))) {
            it->draw();
            it->update();
            ++it;
        } else if (it -> z >= -5.5 && it -> z < 10 && !player.onAir) {
			youLose();
			// printf("YOU LOSE");
			return;
		}
        else {
			WORLD_SPEED += 0.001;
            it = car.erase(it);
            car.insert(it, Car());
        }
    }
}

void displayPaint() {
	// Interactive objects
	handleCar();
	player.handleTrug();
	
	// Static objects
	// Road
	glBindVertexArray(road_VertexArrayID);
	glUniform1i(textureID, 3);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -20.0f));
	MVP = VP * model;
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(modelMatID, 1, GL_FALSE, &model[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, road_vertices.size());
	
	// Sideroad
	glBindVertexArray(sideroad_VertexArrayID);
	glUniform1i(textureID, 5);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-11.0f, 0.0f, 14.0f));
	MVP = VP * model;
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(modelMatID, 1, GL_FALSE, &model[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, sideroad_vertices.size());
	model = glm::translate(glm::mat4(1.0f), glm::vec3(11.0f, 0.0f, 14.0f));
	MVP = VP * model;
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(modelMatID, 1, GL_FALSE, &model[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, sideroad_vertices.size());
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	VP = calculateVP();
	lightDir = glm::vec3(10, 12, -8);
	glUniform3f(lightID, lightDir.x, lightDir.y, lightDir.z);
	displayPaint();
	glfwSwapBuffers(window);
	glfwPollEvents();
	carmov += 0.01;
}

int main() {
	glInit();
	float mult = 1.0f;
	glClearColor(mult*0.973f, mult*0.314f, mult*0.072f, 1.0f);
	initShaders();
	loadTexture();
	loadOBJ();
	initVAO();
	glfwSetKeyCallback(window, key_callback);
	while ((glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) 
			&& (glfwWindowShouldClose(window) == 0) && game)
		display();
	glDeleteBuffers(1, &car_vertexbuffer);
	glDeleteVertexArrays(1, &car_VertexArrayID);
	glDeleteBuffers(1, &road_vertexbuffer);
	glDeleteVertexArrays(1, &road_VertexArrayID);
	glDeleteProgram(programID);
	glDeleteTextures(1, &texture0);
	glDeleteTextures(1, &texture1);
	glfwTerminate();
	return 0;
}