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

	// Stores the x and y coordinates for position of the current character,
	float cursor[2] = {((const guiText *)gui)->bounds.x, ((const guiText *)gui)->bounds.y};


	// Bind the sprite we're using!
	glBindVertexArray(g_spriteDefault.vertexArrayID);

	glActiveTexture(GL_TEXTURE0);
	// Bind the texture that this system uses!
	glBindTexture(GL_TEXTURE_2D, ((const guiText *)gui)->font->atlas->id);


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

			#warning "The top-left corner of the sprite should be at the cursor."
			#warning "We can translate it to that position like so, but what if we used a special sprite?"
			// "curGlyph.uvOffsets.w * 512.f" and "curGlyph.uvOffsets.h * 512.f" get half the glyph's width and height in pixels.
			mat4InitTranslate(&curState->state, cursor[0] + curGlyph.uvOffsets.w * 512.f + curGlyph.kerningX, cursor[1] - curGlyph.uvOffsets.h * 512.f - curGlyph.kerningY, 0.f);
			mat4Scale(&curState->state, curGlyph.uvOffsets.w * 1024.f, curGlyph.uvOffsets.h * 1024.f, 1.f);
			curState->uvOffsets = curGlyph.uvOffsets;

			++numChars;
			// Render everything if we've filled the state buffer.
			#warning "This may be a problem, especially if we're multithreading and other systems are using the same sprite."
			#warning "Since multithreading isn't an option, maybe use a global particle state buffer for all systems?"
			if(numChars >= SPRITE_MAX_INSTANCES){
				// Upload the characters' states to the shader.
				glBindBuffer(GL_ARRAY_BUFFER, g_spriteDefault.stateBufferID);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(charStates) * numChars, &charStates[0]);
				// Draw each character!
				glDrawElementsInstanced(GL_TRIANGLES, g_spriteDefault.numIndices, GL_UNSIGNED_INT, NULL, numChars);

				curState = charStates;
				numChars = 0;
			}else{
				++curState;
			}

			cursor[0] += curGlyph.advanceX;
			// If we've exceeded the width of the
			// text box, move onto the next line.
			if(cursor[0] > ((const guiText *)gui)->bounds.w){
				cursor[0] = ((const guiText *)gui)->bounds.x;
				cursor[1] -= 48.f;//advanceY;
			}
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