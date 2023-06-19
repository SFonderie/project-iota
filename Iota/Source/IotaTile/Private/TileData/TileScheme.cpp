// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileData/TileScheme.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileScheme)

uint8 operator*(ETileScheme Scheme)
{
	return uint8(Scheme);
}

int32 operator<<(int32 Integer, ETileScheme Scheme)
{
	return Integer << uint8(Scheme);
}
