// From ammo.js/tests/stress.js
function stress(Ammo) {
  // Stress test
  function benchmark() {
    var collisionConfiguration = new Ammo.btDefaultCollisionConfiguration();
    var dispatcher = new Ammo.btCollisionDispatcher(collisionConfiguration);
    var overlappingPairCache = new Ammo.btDbvtBroadphase();
    var solver = new Ammo.btSequentialImpulseConstraintSolver();
    var dynamicsWorld = new Ammo.btDiscreteDynamicsWorld(
      dispatcher,
      overlappingPairCache,
      solver,
      collisionConfiguration
    );
    dynamicsWorld.setGravity(new Ammo.btVector3(0, -10, 0));

    var groundShape = new Ammo.btBoxShape(new Ammo.btVector3(50, 50, 50));

    var bodies = [];

    var groundTransform = new Ammo.btTransform();
    groundTransform.setIdentity();
    groundTransform.setOrigin(new Ammo.btVector3(0, -56, 0));

    (function() {
      var mass = 0;
      var localInertia = new Ammo.btVector3(0, 0, 0);
      var myMotionState = new Ammo.btDefaultMotionState(groundTransform);
      var rbInfo = new Ammo.btRigidBodyConstructionInfo(
        0,
        myMotionState,
        groundShape,
        localInertia
      );
      var body = new Ammo.btRigidBody(rbInfo);

      dynamicsWorld.addRigidBody(body);
      bodies.push(body);
    })();

    var sphereShape = new Ammo.btSphereShape(1);
    var boxShape = new Ammo.btBoxShape(new Ammo.btVector3(1, 1, 1));
    var coneShape = new Ammo.btConeShape(1, 1); // XXX TODO: add cylindershape too

    [
      sphereShape,
      boxShape,
      coneShape,
      boxShape,
      sphereShape,
      coneShape
    ].forEach(function(shape, i) {
      var startTransform = new Ammo.btTransform();
      startTransform.setIdentity();
      var mass = 1;
      var localInertia = new Ammo.btVector3(0, 0, 0);
      shape.calculateLocalInertia(mass, localInertia);

      startTransform.setOrigin(
        new Ammo.btVector3(2 + i * 0.01, 10 + i * 2.1, 0)
      );

      var myMotionState = new Ammo.btDefaultMotionState(startTransform);
      var rbInfo = new Ammo.btRigidBodyConstructionInfo(
        mass,
        myMotionState,
        shape,
        localInertia
      );
      var body = new Ammo.btRigidBody(rbInfo);

      dynamicsWorld.addRigidBody(body);
      bodies.push(body);
    });

    var startTime = Date.now();

    var NUM = 150000;

    for (var i = 0; i < NUM; i++) {
      dynamicsWorld.stepSimulation(1 / 60, 10);
    }

    var endTime = Date.now();
    return (endTime - startTime) / 1000;
  }

  return benchmark();
}

module.exports = stress;
