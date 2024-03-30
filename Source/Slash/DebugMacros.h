#pragma once

#include<DrawDebugHelpers.h>

#define MY_DEBUG_SPHERE( Location ) { if( GetWorld( ) ) DrawDebugSphere( GetWorld( ) , Location , 25.0f , 12 , FColor :: Red , true ) ; } // If true so debug sphere will never go away
#define DRAW_LINE( Start_Location , End_Location ) { if( GetWorld( ) ) DrawDebugLine( GetWorld( ) , Start_Location , End_Location , FColor :: Red , true , -1.0f , 0 , 1.0f ) ; } 
#define DRAW_POINT( Location ) { if( GetWorld( ) ) DrawDebugPoint( GetWorld() , Location , 15.0f, FColor :: Red, true ); }
// BackSlash allows macro definition to be entered in new line also.
#define DRAW_VECTOR( Start_Location , End_Location ) if ( GetWorld( ) ) \
{     DrawDebugLine(GetWorld(), Start_Location, End_Location, FColor :: Red, true, -1.0f, 0, 1.0f); \
	  DrawDebugPoint(GetWorld(), End_Location, 15.0f, FColor :: Red, true); \
}
