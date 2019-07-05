/****************************************************************************

							  Region.cpp
						Copyright 1998, John J. Bolton

	----------------------------------------------------------------------

	$Header: //depot/Libraries/Region/Region.cpp#2 $

	$NoKeywords: $

 ****************************************************************************/

#include "Region.h"

#include <Cassert>

using namespace std;

/********************************************************************************************************************/
/*																													*/
/* Region::Region()														*/
/*																													*/
/********************************************************************************************************************/

//Region::Region()
//{
//}


/********************************************************************************************************************/
/*																													*/
/* Region::~Region()														*/
/*																													*/
/********************************************************************************************************************/
Region::~Region()
{
}


/********************************************************************************************************************/
/*																													*/
/* Region::Include()														*/
/*																													*/
/********************************************************************************************************************/

void Region::Include( Rect add_rect )
{
	IncludeWithoutJoining( add_rect );

	Join();
}

/********************************************************************************************************************/
/*																													*/
/* Region::Include()														*/
/*																													*/
/********************************************************************************************************************/

void Region::Include( RectList const & rl )
{
	assert( &rl != this );

	for ( const_iterator ait = rl.begin(); ait != rl.end(); ait++ )
		IncludeWithoutJoining( *ait );

	Join();
}


/********************************************************************************************************************/
/*																													*/
/* Region::Consolidate()													*/
/*																													*/
/********************************************************************************************************************/

void Region::Consolidate()
{
	RectList copy_list;

	swap( copy_list );

	Include( copy_list );
}


/********************************************************************************************************************/
/*																													*/
/* Region::IncludeWithoutJoining()											*/
/*																													*/
/********************************************************************************************************************/

void Region::IncludeWithoutJoining( Rect add_rect )
{
	iterator i;

		// If add_rect contains any rects in the region, then erase them

	i = begin();
	while ( i != end() )
	{
		if ( add_rect.Contains( *i ) )
			i = erase( i );
		else
			++i;
	}

		// The resulting list of rects after splitting add_rect
	RectList add_list;
	add_list.push_back( add_rect );

		// Go through the list. Every rect that overlaps a subrect of add_rect splits it.
	
	for ( i = begin(); i != end(); i++ )
	{
		RectList::iterator j = add_list.begin();
		while ( j != add_list.end() )
		{
				// If this rect is completely contained in another then it is not needed

			if ( i->Contains( *j ) )
				j = add_list.erase( j );

				// If this rect overlaps another, then split it and keep the pieces that
				// aren't contained by the splitting rect

			else if ( i->Overlaps( *j ) )
			{
				RectVector split_vector;
				split_vector.reserve( 6 );		// Preallocate to boost preformance (6 is the max)

				split_vector.push_back( *j );
				SplitVertically( split_vector, i->m_X );
				SplitVertically( split_vector, i->m_X + i->m_Width );
				SplitHorizontally( split_vector, i->m_Y );
				SplitHorizontally( split_vector, i->m_Y + i->m_Height );

					// Replace the split rect by its pieces

				j = add_list.erase( j );

				for ( RectVector::iterator k = split_vector.begin(); k != split_vector.end(); k++ )
				{
					if ( !i->Contains( *k ) )
						add_list.insert( j, *k );
				}
			}

				// Otherwise go on

			else
			{
				++j;
			}
		}
	}

		// Append the add_list

	insert( end(), add_list.begin(), add_list.end() );
}

/********************************************************************************************************************/
/*																													*/
/* Region::Join()															*/
/*																													*/
/********************************************************************************************************************/

void Region::Join()
{
		// Repeatedly go through the list and join any rects until no more
		// are joined.

	bool rects_were_joined;

	do
	{
		rects_were_joined = false;		// Assume nothing happens

		for ( iterator i = begin(); i != end(); i++ )
		{
			iterator j = i;
			++j;

			while ( j != end() )
			{
					// There should be no completely contained rects

				assert( !i->Contains( *j ) );

					// If three sides match then this rect can be combined to make a bigger rect

				if ( ( i->m_X == j->m_X && i->m_X+i->m_Width == j->m_X+j->m_Width && (  i->m_Y == j->m_Y+j->m_Height || i->m_Y+i->m_Height == j->m_Y ) ) ||
					 ( i->m_Y == j->m_Y && i->m_Y+i->m_Height == j->m_Y+j->m_Height && (  i->m_X == j->m_X+j->m_Width || i->m_X+i->m_Width == j->m_X ) ) )
				{
					i->Include( *j );
					j = erase( j );
					rects_were_joined = true;
				}
				else
				{
					++j;
				}
			}
		}
	} while ( rects_were_joined );
}


/********************************************************************************************************************/
/*																													*/
/* Region::SplitVertically()												*/
/*																													*/
/********************************************************************************************************************/

void Region::SplitVertically( RectVector& v, int x )
{
	const int n = v.size();

	for ( int i = 0; i < n; i++ )
	{
		Rect& r = v[i];

		if ( r.m_X < x && x < r.m_X + r.m_Width )
		{
			const int new_width = x - r.m_X;

				// Add the right side as a new rect

			push_back( Rect( x, r.m_Y, r.m_Width - new_width, r.m_Height ) );

				// Clip the left side

			r.m_Width = new_width;
		}
	}
}


/********************************************************************************************************************/
/*																													*/
/* Region::SplitHorizontally()												*/
/*																													*/
/********************************************************************************************************************/

void Region::SplitHorizontally( RectVector& v, int y )
{
	const int n = v.size();

	for ( int i = 0; i < n; i++ )
	{
		Rect& r = v[i];

		if ( r.m_Y < y && y < r.m_Y + r.m_Height )
		{
			const int new_width = y - r.m_Y;

				// Add the right side as a new rect

			push_back( Rect( y, r.m_Y, r.m_Height - new_width, r.m_Height ) );

				// Clip the left side

			r.m_Height = new_width;
		}
	}
}

