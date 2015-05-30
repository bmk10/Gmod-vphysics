package jiglib.vehicles
{
	import jiglib.geometry.JBox;
	import jiglib.plugin.ISkin3D;

	public class JChassis2 extends JBox
	{

		private var _car:JCar2;

		public function JChassis2(car:JCar2, skin:ISkin3D, width:Number = 40, depth:Number = 70, height:Number = 30)
		{
			super(skin, width, depth, height);

			_car = car;
		}

		public function get car():JCar2
		{
			return _car;
		}

		override public function postPhysics(dt:Number):void
		{
			super.postPhysics(dt);
			_car.addExternalForces(dt);
			_car.postPhysics(dt);
		}
	}
}
