#pragma once

#include<DrawDebugHelpers.h>

#define DRAW_SPHERE( Location ) { if( GetWorld( ) ) DrawDebugSphere( GetWorld( ) , Location , 25.0f , 12 , FColor :: Red , true ) ; } // If true so debug sphere will never go away
#define DRAW_SPHERE_SINGLE_FRAME( Location ) { if( GetWorld( ) ) DrawDebugSphere( GetWorld( ) , Location , 25.0f , 12 , FColor :: Red , false , -1.0f ) ; }
#define DRAW_LINE( Start_Location , End_Location ) { if( GetWorld( ) ) DrawDebugLine( GetWorld( ) , Start_Location , End_Location , FColor :: Red , true , -1.0f , 0 , 1.0f ) ; } 
#define DRAW_LINE_SINGLE_FRAME( Location ) { if( GetWorld( ) ) DrawDebugLine( GetWorld( ) , Start_Location , End_Location , FColor :: Red , false , -1.0f , 0 , 1.0f ) ; } 
#define DRAW_POINT( Location ) { if( GetWorld( ) ) DrawDebugPoint( GetWorld() , Location , 15.0f, FColor :: Red, true ); }
#define DRAW_POINT_SINGLE_FRAME( Location ) { if( GetWorld( ) ) DrawDebugPoint( GetWorld() , Location , 15.0f, FColor :: Red, false , -1.0f ); }
#define DRAW_VECTOR( Start_Location , End_Location ) if ( GetWorld( ) ) \
{     DrawDebugLine(GetWorld(), Start_Location, End_Location, FColor :: Red, true, -1.0f, 0, 1.0f); \
	  DrawDebugPoint(GetWorld(), End_Location, 15.0f, FColor :: Red, true); \
}
#define DRAW_VECTOR_SINGLE_FRAME( Start_Location , End_Location ) if ( GetWorld( ) ) \
{     DrawDebugLine(GetWorld(), Start_Location, End_Location, FColor :: Red, false, -1.0f, 0, 1.0f); \
	  DrawDebugPoint(GetWorld(), End_Location, 15.0f, FColor :: Red, false , -1.0f ); \
}
#define DRAW_ARROW( Start_Location , End_Location ) { if( GetWorld( ) ) DrawDebugDirectionalArrow(GetWorld( ) , Start_Location, End_Location , 25.0f, FColor :: Magenta, true , -1.0f , 0 , 1.5f );}
#define DRAW_BOX( Location , Extent ) { if( GetWorld() ) DrawDebugBox(GetWorld( ) , Location, Extent , FColor::Blue, true, -1.0f, 0, 1.0f); }
#define DRAW_COORDINATES( Location , Rotation ) { if( GetWorld( ) ) DrawDebugCoordinateSystem(GetWorld(), Location, Rotation, 150.f, true , -1.0f , 0 , 1.0f ); }
