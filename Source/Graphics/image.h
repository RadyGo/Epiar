/**\file			image.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Saturday, January 31, 2009
 * \date			Modified: Saturday, November 21, 2009
 * \brief			Image loading and display
 * \details
 * You don't have to worry about OpenGL's power of 2 image dimension requirements.
 * This class will scale for you while still displaying correctly.
 * When editing this class, there are a number of curious variables and conventions to
 * pay attention to. The real width/height of the image (rw, rh) is rarely used - it is
 * the true width and height of the image according to OpenGL. However, as we often expand
 * images whose dimensions weren't a power of two, but must be for proper OpenGL texture
 * size requirements, we may internally expand the dimensions of the image (thus the rw,rh grow)
 * but the w/h is still effectively (at least as far as we care on the outside) whatever non-
 * power of two dimensions, and these effective, "fake" dimensions are called it's _virtual_
 * dimensions, or virtual width/height, stored in w, h.
 */

#ifndef __H_IMAGE__
#define __H_IMAGE__

#include "includes.h"
#include "Utilities/resource.h"

class Image : public Resource {
	public:
		Image();
		// Create instance by loading image from file
		Image( const string& filename );
		~Image();

		static Image* Get(string filename);

		// Load image from file
		bool Load( const string& filename );
		// Load image from buffer
		bool Load( char *buf, int bufSize );

		// Get information about image dimensions (always the virtual/effective size)
		int GetWidth( void ) { return w; };
		int GetHeight( void ) { return h; };
		int GetHalfWidth( void ) { return w / 2; };
		int GetHalfHeight( void ) { return h / 2; };

		// Draw the image (angle in degrees)
		void Draw( int x, int y, float angle = 0. );
		// Draw the image centered on (x,y) (angle in degrees)
		void DrawCentered( int x, int y, float angle = 0. );
		// Draw the image tiled to fill a rectangle of w/h - will crop to meet w/h and won't overflow
		void DrawTiled( int x, int y, int w, int h );
		
		// resizes the image by stretching the GL quad at draw time
		void Resize( int w, int h );

	private:
		// Converts an SDL surface to an OpenGL texture
		bool ConvertToTexture( SDL_Surface *s );
		// Expands surface 's' to width/height of w/h, keeping the original image in the upper-left
		SDL_Surface *ExpandCanvas( SDL_Surface *s, int w, int h );
		// Returns the next highest power of two if num is not a power of two
		int PowerOfTwo(int num);

		int w, h; // virtual w/h (effective, same as original file)
		int real_w, real_h; // real w/h, size of expanded canvas (image) should expansion be needed
		            //   to meet power of two requirements
		float scale_w, scale_h; // used by draw() on images with expanded canvases to know what percent of
		                        // the larger canvas actually contains the original image (<= 1.0)
		                        // defaults = 1.0, this factor is always used, so non-expanded images are
		                        // simply "scaled" at 1.0. THIS HAS NOTHING TO DO WITH RESIZE()
		float resize_ratio_w, resize_ratio_h; // THIS HAS EVERYTHING TO DO WITH RESIZE() (see above for the humor)
		GLuint image; // OpenGL pointer to texture
};

#endif // __H_IMAGE__
