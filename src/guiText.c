#include "guiText.h"


#include <stddef.h>

#include "vec2.h"

#include "texture.h"
#include "sprite.h"


void guiTextInit(guiText *const restrict gui, const textFont *const restrict font, const size_t bufferSize){
	gui->font = font;

	textBufferInit(&gui->text, bufferSize);

	gui->bounds.x =
	gui->bounds.y =
	gui->bounds.w =
	gui->bounds.h = 0.f;
}


void guiTextUpdate(void *const restrict gui, const float time){
	//
}

#warning "To correctly handle size changes mid-text, we need to know the largest font size of the following line."
void guiTextDraw(
	const void *const restrict gui, const transformState *const restrict root, const shaderSprite *const restrict shader
){

	#warning "If we use a global sprite buffer, should we make this global too?"
	spriteState charStates[SPRITE_MAX_INSTANCES];
	spriteState *curState = charStates;
	size_t numChars = 0;

	const byte_t *curChar = ((const guiText *)gui)->text.offset;
	const texture *curAtlas = NULL;

	// Stores the x and y coordinates for position of the current character,
	float cursor[2] = {((const guiText *)gui)->bounds.x, ((const guiText *)gui)->bounds.y};


	// Bind the sprite we're using!
	glBindVertexArray(g_spriteDefault.vertexArrayID);
	glActiveTexture(GL_TEXTURE0);


	// Store our text data in the arrays.
	do {
		const textCMapCodeUnit_t code = {._32 = textBufferRead(&(((const guiText *)gui)->text), &curChar)};
		// End of stream.
		if(code._32 == 0){
			break;

		// New line.
		}else if(code._32 == '\n'){
			cursor[0] = ((const guiText *)gui)->bounds.x;
			cursor[1] -= 48.f;//advanceY * format.size;

		// An invalid character code usually indicates a change in text formatting.
		}else if(code._32 == TEXT_UTF8_INVALID_CODE){
			//

		// Regular character.
		}else{
			const textGlyph curGlyph = *textFontGetGlyph(((const guiText *)gui)->font, code);

			// If the current glyph uses a new texture atlas, render
			// all of the characters in our buffer and reset it.
			if(curAtlas != curGlyph.atlas){
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
			if(cursor[0] + curGlyph.advanceX > ((const guiText *)gui)->bounds.w){
				cursor[0] = ((const guiText *)gui)->bounds.x;
				cursor[1] -= 48.f;//advanceY * format.size;
			}


			#warning "The top-left corner of the sprite should be at the cursor."
			#warning "We can translate it to that position like so, but what if we used a special sprite?"
			// We need to translate the glyph such that its origin is the
			// top-left corner of the sprite for our kerning to work correctly.
			mat4InitTranslate(
				&curState->state,
				cursor[0] + (curGlyph.uvOffsets.w * (curAtlas->width >> 1)) + curGlyph.kerningX,
				cursor[1] - (curGlyph.uvOffsets.h * (curAtlas->height >> 1)) - curGlyph.kerningY,
				0.f
			);
			mat4Scale(&curState->state, curGlyph.uvOffsets.w * curAtlas->width, curGlyph.uvOffsets.h * curAtlas->height, 1.f);
			curState->uvOffsets = curGlyph.uvOffsets;


			++numChars;
			// If the buffer is full, set it to render the next time we read a character.
			#warning "This may be a problem, especially if we're multithreading and other systems are using the same sprite."
			#warning "Since multithreading isn't an option, maybe use a global particle state buffer for all systems?"
			if(numChars >= SPRITE_MAX_INSTANCES){
				curAtlas = NULL;
			}else{
				++curState;
			}

			// Move the cursor.
			cursor[0] += curGlyph.advanceX;
		}
	} while(curChar != ((const guiText *)gui)->text.offset);


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


void guiTextDelete(void *const restrict gui){
	textBufferDelete(&(((guiText *)gui)->text));
}