#include "pch.h"
#include "ACameraActor.h"

//void ACameraActor::MoveSplineCurve()
//{
//	Vec3 start = Vec3(0.f, 20.f, 0.f);
//	Vec3 middle1 = Vec3(25.f, 20.f, 0.f);
//    Vec3 middle2 = Vec3(25.f, 20.f, 0.f);
//	Vec3 end = Vec3(50.f, 20.f, 0.f);
//
//    static float fcurrentTime = 0.f;
//    fcurrentTime += TIMER->GetDeltaTime();
//
//    // Loop
//    if (fcurrentTime > m_iMoveTime)
//        fcurrentTime = 0.f;
//
//    float tempTime = fcurrentTime / m_iMoveTime;
//
//    SetPosition(BezierCubic(start, middle1, middle2, end, tempTime));
//}