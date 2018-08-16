class Show {
  constructor() {
    // Websocket
    this.ws = new WebSocket("ws://localhost:8081");
    this.ws.onclose = function() {
      console.log("Websocket closed!");
    }

    // Renderer
    this.width = 680;
    this.height = 480;
    this.aspect_ratio = this.width / this.height;
    this.renderer = new THREE.WebGLRenderer({preserveDrawingBuffer: true});
    this.renderer.setSize(this.width, this.height);
    var container = document.getElementById("show_container");
    container.appendChild(this.renderer.domElement);

    // Stats
    this.stats = new Stats();
    container.appendChild(this.stats.dom);

    // Camera
    this.camera = new THREE.PerspectiveCamera(70, this.aspect_ratio, 1, 10000);
    this.camera.up.set(0, 0, 1);
    this.camera.position.x = -300;
    this.camera.position.y = -300;
    this.camera.position.z = 500;

    // Cube
    var geometry = new THREE.BoxGeometry(10, 10, 10);
    var material = new THREE.MeshBasicMaterial({color: 0x00ff00});
    this.mesh = new THREE.Mesh(geometry, material);

    // Camera controls
    this.controls = new THREE.OrbitControls(this.camera, this.renderer.domElement);
    this.controls.maxPolarAngle = Math.PI / 2;
    this.controls.keys = {
      LEFT: 37, //left arrow
      UP: 38, // up arrow
      RIGHT: 39, // right arrow
      BOTTOM: 40 // down arrow
    };
    this.controls.update();

    // Local frame
    var axes = new THREE.AxesHelper(50);
    axes.material.linewidth = 3;

    // Ground plain grid
    var gridXZ = new THREE.GridHelper(500, 10);
    gridXZ.rotation.x = 90.0 * Math.PI / 180;

    // Scene
    this.scene = new THREE.Scene();
    // this.scene.background = new THREE.Color(0xcccccc);
    this.scene.background = new THREE.Color(0x000000);
    // this.scene.add(this.mesh);
    this.scene.add(axes);
    this.scene.add(gridXZ);

    // // Ambient light
    // var ambient_light = new THREE.AmbientLight(0xcccccc, 1);
    // scene.add(ambient_light);

    // Directional light
    // var directional_light = new THREE.DirectionalLight(0xffffff, 0.8);
    // directional_light.up.set(0, 0, 1);
    // directional_light.position.set(0, 1, 1).normalize();
    // scene.add(directional_light);

    // Loading manager
    var quad_model;
    var building_model;
    var scene = this.scene;
    var loading_manager = new THREE.LoadingManager(function() {
      scene.add(quad_model);
      scene.add(building_model);
    });

    // Collada model loader
    var collada_loader = new THREE.ColladaLoader(loading_manager);
    collada_loader.load('./models/quadrotor/quadrotor.dae', function(collada) {
      quad_model = collada.scene;
      quad_model.scale.set(50, 50, 50);
      quad_model.position.set(0, 0, 10);
      quad_model.rotation.set(0, 0, 0);
    });

    collada_loader.load('./models/small_building/small_building.dae', function(collada) {
      building_model = collada.scene;
      building_model.scale.set(50, 50, 50);
      building_model.position.set(200, 0, 0);
      building_model.rotation.set(0, 0, 0);
    });


		// Point cloud test
		var point_geometry = new THREE.Geometry();
		for ( var i = 0; i < 2000000; i ++ ) {
			var point = new THREE.Vector3();
			point.x = THREE.Math.randFloatSpread( 20000 );
			point.y = THREE.Math.randFloatSpread( 20000 );
			point.z = THREE.Math.randFloatSpread( 20000 );
			point_geometry.vertices.push(point);
		}
		var point_material = new THREE.PointsMaterial({color: 0xffffff});
		var point_cloud = new THREE.Points(point_geometry, point_material);
		scene.add(point_cloud);

		// DO NOT DO IT THIS WAY ELSE THE RENDER WILL BE 1-2 FPS
		// var limit = 20000;
		// for (var i = 0; i < limit; i++) {
		// 	var point_geometry = new THREE.Geometry();
    //
		// 	var upper_bound = 400;
		// 	var lower_bound = -400;
		// 	var x = Math.floor(Math.random() * (upper_bound - lower_bound + 1)) + lower_bound;
		// 	var y = Math.floor(Math.random() * (upper_bound - lower_bound + 1)) + lower_bound;
		// 	var z = Math.floor(Math.random() * (upper_bound - lower_bound + 1)) + lower_bound;
    //
		// 	point_geometry.vertices.push(new THREE.Vector3(x, y, z));
		// 	var point_material = new THREE.PointsMaterial( { size: 1, sizeAttenuation: false } );
		// 	var point = new THREE.Points( point_geometry, point_material );
    //
		// 	scene.add(point);
		// }

    // Events
    window.addEventListener('resize', function() {that.onWindowResize();}, false);
  }

  onWindowResize() {
    this.camera.aspect = window.innerWidth / window.innerHeight;
    this.camera.updateProjectionMatrix();
    this.renderer.setSize(window.innerWidth, window.innerHeight);
  }

  render() {
    this.mesh.rotation.x += 0.005;
    this.mesh.rotation.y += 0.01;
    this.controls.update();
    this.renderer.render(this.scene, this.camera);

    var that = this;
    requestAnimationFrame(function() {that.render();});
    this.stats.update();
    this.publishScene();
  }

  publishScene() {
    var mime_type = "image/png";
    var imgData = this.renderer.domElement.toDataURL(mime_type);
    if (this.ws.readyState == 1) {
      this.ws.send(imgData);
    }
  }
}

let show = new Show();
show.render();
