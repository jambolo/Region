/****************************************************************************

							   Region.h
						Copyright 1998, John J. Bolton

	----------------------------------------------------------------------

	$Header: //depot/Libraries/Region/Region.h#2 $

	$NoKeywords: $

 ****************************************************************************/

#pragma once

#include <list>
#include <vector>

#include "../Rect/Rect.h"

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class Region : public RectList
{
public:

		// Constructor
	Region() {}

		// Constructor
	Region( RectList const & rl ) : RectList( rl ) {}

		// Copy constructor
//	Region( Region const & copy );

		// Assignment operator
//	Region& operator=( Region const & rvalue )

		// Destructor
	virtual ~Region();

		// Adds the given rect to the list
	virtual void Include( Rect rect );

		// Adds the given rect list to the list
	virtual void Include( RectList const & rl );

		// Remove any portions of rects which overlap other rects in the list
	void Consolidate();

protected:

private:
	typedef std::vector< Rect > RectVector;

		// Go through all the rects in the vector and split them horizontally
		// at y into 2 rects, the first replaces the original, the second is appended.
	void SplitHorizontally( RectVector& v, int y );

		// Go through all the rects in the vector and split them vertically
		// at x into 2 rects, the first replaces the original, the second is appended.
	void SplitVertically( RectVector& v, int x );

		// Combine all rects that have 3 sides in common
	void Join();

		// Include a rect without joining it
	void IncludeWithoutJoining( Rect add_rect );
};
