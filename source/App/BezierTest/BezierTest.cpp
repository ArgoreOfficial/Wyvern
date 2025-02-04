#include "BezierTest.h"
#include <wv/Scene/Scene.h>
#include "wv/Engine/ApplicationState.h"
#include "wv/Math/BezierCurve.h"
#include <wv/Scene/Component/ModelComponent.h>
#include <wv/Scene/Component/RigidBodyComponent.h>
#include "wv/Debug/Draw.h"
#include "wv/Memory/Memory.h"

BezierTest::BezierTest()
{


    wv::Scene* scene = wv::cEngine::get()->m_pApplicationState->getCurrentScene();

    wv::sPhysicsBoxDesc* boxDesc = WV_NEW(wv::sPhysicsBoxDesc);
    boxDesc->kind = wv::WV_PHYSICS_DYANIMIC;
    boxDesc->halfExtent = { 0.5f,0.5f,0.5f };

    wv::cRigidbody* rb = WV_NEW(wv::cRigidbody, wv::cEngine::getUniqueUUID(), "cube");
    rb->addComponent<wv::RigidBodyComponent>(boxDesc);
    rb->addComponent<wv::ModelComponent>("meshes/cube.dae");

    scene->addChild(rb);

    scene->onEnter();
    scene->onConstruct();

    bezier = wv::BezierCurve<float>({
        {0.0f, 0.0f, 0.0f},
        {10.0f, 20.0f,15.0f},
        {30.0f, 30.0f, 0.0f},
        {40.0f, 700.0f, 10.0f}
        });

    system("clear");
}

BezierTest::~BezierTest() {}

void BezierTest::Update(double deltaTime)
{
    if (t <= 1.0f)
    {

        wv::Vector3f position = bezier.evaluate(t);

        rb->m_transform.setPosition({ position.x, position.z, position.y });

        t += speed * (deltaTime);
    }

}
