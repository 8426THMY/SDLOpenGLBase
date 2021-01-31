#include "guiText.h"


#include <stddef.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include "vec2.h"

#include "texture.h"

#include "guiElement.h"


void guiTextInit(guiText *const restrict gui, const textFont *const restrict font, const size_t bufferSize){
	textBufferInit(&gui->buffer, bufferSize);
	gui->font = font;

	gui->width  =
	gui->height = 0.f;
}


void guiTextUpdate(guiElement *const restrict gui, const float time){
	//
}

#warning "To correctly handle size changes mid-text, we need to know the largest font size of the following line."
void guiTextDraw(
	const guiElement *const restrict gui, const spriteShader *const restrict shader
){

	if(!textBufferIsEmpty(&gui->data.text.buffer)){
		const guiText text = gui->data.text;
		#warning "If we use a global sprite buffer, should we make this global too?"
		spriteState charStates[SPRITE_MAX_INSTANCES];
		spriteState *curState = charStates;
		size_t numChars = 0;

		const byte_t *curChar = textBufferGetStart(&text.buffer);
		const texture *curAtlas = NULL;

		// Stores the x and y coordinates for position of the current character,
		float cursor[2] = {0.f, 0.f};
		mat4 rootTransform;
		transformStateToMat4(&gui->root, &rootTransform);


		// Send the font's image type and colours to the shader!
		glUniform1ui(shader->sdfTypeID, text.font->type);
		if(text.font->type != SPRITE_IMAGE_TYPE_NORMAL){
			glUniform4f(shader->sdfColourID, 1.f, 1.f, 1.f, 1.f);
			glUniform4f(shader->sdfBackgroundID, 1.f, 0.5f, 0.f, 1.f);
		}

		// Bind the sprite we're using!
		glBindVertexArray(g_spriteDefault.vertexArrayID);
		glActiveTexture(GL_TEXTURE0);


		// Store our text data in the arrays.
		do {
			const textCMapCodeUnit_t code = {._32 = textBufferReadUTF8(&text.buffer, &curChar)};
			// End of stream.
			if(code._32 == 0){
				break;

			// New line.
			}else if(code._32 == '\n'){
				cursor[0] = 0.f;
				cursor[1] -= 48.f;//advanceY * format.size;

			// An invalid character code usually indicates either a change in text
			// formatting or part of a multi-byte UTF-8 code point was overwritten.
			}else if(code._32 == TEXT_UTF8_INVALID_CODE){
				//

			// Regular character.
			}else{
				const textGlyph curGlyph = *textFontGetGlyph(text.font, code);
				const float halfWidth = curGlyph.uvOffsets.w * (curGlyph.atlas->width >> 1);
				float translateX = cursor[0] + halfWidth + curGlyph.kerningX;
				float translateY = -(curGlyph.uvOffsets.h * (curGlyph.atlas->height >> 1)) - curGlyph.kerningY;

				// If we've filled the render buffer or the new character uses a different
				// texture atlas, draw all of the characters in our buffer and empty it.
				#warning "This may be a problem, especially if we're multithreading and other systems are using the same sprite."
				#warning "Since multithreading isn't an option, maybe use a global particle state buffer for all systems?"
				if(numChars >= SPRITE_MAX_INSTANCES || curAtlas != curGlyph.atlas){
					if(numChars != 0){
						// Upload the characters' states to the shader.
						glBindBuffer(GL_ARRAY_BUFFER, g_spriteDefault.stateBufferID);
						glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(*curState) * numChars, &charStates[0]);
						// Draw each character!
						glDrawElementsInstanced(GL_TRIANGLES, g_spriteDefault.numIndices, GL_UNSIGNED_INT, NULL, numChars);

						curState = charStates;
						numChars = 0;
					}

					curAtlas = curGlyph.atlas;
					// Bind the texture that this glyph uses!
					glBindTexture(GL_TEXTURE_2D, curAtlas->id);
				}


				// If we've exceeded the width of the text box, move onto the next line.
				if((translateX + halfWidth)*gui->root.scale.x > text.width){
					translateX -= cursor[0];
					cursor[0] = 0.f;
					cursor[1] -= 48.f;//advanceY * format.size;
				}
				translateY += cursor[1];


				#warning "The top-left corner of the sprite should be at the cursor."
				#warning "We can translate it to that position like so, but what if we used a special sprite?"
				// We need to translate the glyph such that its origin is the
				// top-left corner of the sprite for our kerning to work correctly.
				curState->state = rootTransform;
				mat4Translate(&curState->state, translateX, translateY, 0.f);
				mat4Scale(&curState->state, curGlyph.uvOffsets.w * curAtlas->width, curGlyph.uvOffsets.h * curAtlas->height, 1.f);
				curState->uvOffsets = curGlyph.uvOffsets;

				++numChars;
				++curState;

				// Move the cursor.
				cursor[0] += curGlyph.advanceX;
			}
		} while(!textBufferFinishedReading(&text.buffer, curChar));


		#warning "This may be a problem, especially if we're multithreading and other systems are using the same sprite."
		#warning "Since multithreading isn't an option, maybe use a global particle state buffer for all systems?"
		// Render any leftover characters.
		if(numChars > 0){
			// Upload the characters' states to the shader.
			glBindBuffer(GL_ARRAY_BUFFER, g_spriteDefault.stateBufferID);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(*curState) * numChars, &charStates[0]);
			// Draw each character!
			glDrawElementsInstanced(GL_TRIANGLES, g_spriteDefault.numIndices, GL_UNSIGNED_INT, NULL, numChars);
		}
	}
}


void guiTextDelete(guiElement *const restrict gui){
	textBufferDelete(&(gui->data.text.buffer));
}