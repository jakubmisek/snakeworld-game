#pragma once

GLint iphone_gluUnProject(GLfloat winx, GLfloat winy, GLfloat winz,
						  const GLfloat model[16], const GLfloat proj[16],
						  const GLint viewport[4],
						  GLfloat * objx, GLfloat * objy, GLfloat * objz);
