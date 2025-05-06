#pragma once

#include <tuple>
#include "CCollision.h"
#include "CInput.h"
#include "CLifespan.h"
#include "CScore.h"
#include "CShape.h"
#include "CTransform.h"

using ComponentTuple = std::tuple<
        CCollision,
        CInput,
        CLifespan,
        CScore,
        CShape,
        CTransform
>;