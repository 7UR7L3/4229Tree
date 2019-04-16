//****************************************************//
//							 		//
//		morose.shader for Q3Radiant			//
//		morose site skybox by Speedy 			//
//		www.planetquake.com/speedy			//
//	note: you might need to tweak "q3map_sun_sun"	//
//									//

// Direction & elevation checked and adjusted - Sepaker
//****************************************************//

textures/skies/morose
{
	qer_editorimage env/morose/morose_ft.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_globaltexture
	q3map_lightsubdivide 256
	q3map_surfacelight 50
	surfaceparm sky
	q3map_sun 0.3 0.4 0.65 60 190 70
	skyparms env/morose/morose - -
}
