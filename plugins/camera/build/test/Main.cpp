// Main.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/02/28
// Copyright (C) University of Augsburg, Lab for Human Centered Multimedia
//
// *************************************************************************************************
//
// This file is part of Social Signal Interpretation (SSI) developed at the 
// Lab for Human Centered Multimedia of the University of Augsburg
//
// This library is free software; you can redistribute itand/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or any laterversion.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FORA PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along withthis library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
//*************************************************************************************************

#include "ssi.h"
#include "ssicamera.h"
#include "..\..\..\audio\include\ssiaudio.h"
using namespace ssi;

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

void record ();
bool ex_cam (void *args);
bool ex_camfile (void *args);
bool ex_camscreen (void *args);

int main () {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe");
	Factory::RegisterDLL ("ssievent");
	Factory::RegisterDLL ("ssiioput");
	Factory::RegisterDLL ("ssigraphic");
	Factory::RegisterDLL ("ssicamera");
	Factory::RegisterDLL ("ssiaudio");
	Factory::RegisterDLL ("ssiimage");

	Exsemble ex;
	ex.console(0, 0, 650, 600);
	ex.add(ex_cam, 0, "CAMERA", "Display");
	ex.add(ex_camfile, 0, "CAMERA", "Write to file");
	ex.add(ex_camscreen, 0, "CAMERA", "Capture screen");
	ex.show();

	ssi_print ("\n\n\tpress enter to quit\n");
	getchar ();
	
	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif
	
	return 0;
}

bool ex_cam (void *args) {

	ITheFramework *frame = Factory::GetFramework ();

	Decorator *decorator = ssi_create (Decorator, 0, true);
	frame->AddDecorator(decorator);

	Audio *audio = ssi_create (Audio, "audio", true);	
	ITransformable *audio_p = frame->AddProvider(audio, SSI_AUDIO_PROVIDER_NAME);
	frame->AddSensor(audio);
	
	Camera *camera = ssi_create (Camera, "camera", true);	
	ITransformable *camera_p = frame->AddProvider(camera, SSI_CAMERA_PROVIDER_NAME);
	frame->AddSensor(camera);

	ssi_video_params_t video_params = camera->getOptions()->params;
	
	ITransformable *ids[] = { camera_p, audio_p };
	CameraWriter *cam_write = ssi_create (CameraWriter, 0, true);
	cam_write->getOptions()->setPath("video.avi");
	//cam_write->getOptions()->mirror = false;
	//cam_write->getOptions()->flip = true;
	cam_write->getOptions()->setCompression("Microsoft Video 1");
	//cam_write->getOptions()->setCompression(SSI_CAMERA_USE_NO_COMPRESSION);
	frame->AddConsumer(2, ids, cam_write, "1");

	VideoPainter *camera_plot = ssi_create_id (VideoPainter, 0, "plot");
	camera_plot->getOptions()->mirror = false;
	camera_plot->getOptions()->flip = true;
	camera_plot->getOptions()->setTitle("camera");
	frame->AddConsumer(camera_p, camera_plot, "1");

	SignalPainter *audio_plot = ssi_create_id (SignalPainter, 0, "plot");
	audio_plot->getOptions()->setTitle("audio");
	audio_plot->getOptions()->size = 2.0;	
	audio_plot->getOptions()->type = PaintSignalType::AUDIO;
	frame->AddConsumer(audio_p, audio_plot, "0.1s");

	decorator->add("console", 0, 0, 650, 800);
	decorator->add("plot*", 650, 0, 400, 400);
	decorator->add("monitor*", 650, 400, 400, 400);

	frame->Start();
	frame->Wait();
	frame->Stop();
	frame->Clear();

	return true;
}

void record () {

	ITheFramework *frame = Factory::GetFramework ();

	Decorator *decorator = ssi_create (Decorator, 0, true);
	frame->AddDecorator(decorator);

	Camera *camera = ssi_create (Camera, "camera", true);	
	ITransformable *camera_p = frame->AddProvider(camera, SSI_CAMERA_PROVIDER_NAME);
	frame->AddSensor(camera);
	CameraWriter *cam_write = ssi_create (CameraWriter, 0, true);
	cam_write->getOptions()->setPath("video.avi");
	cam_write->getOptions()->setCompression("Microsoft Video 1");
	//cam_write->getOptions()->setCompression(SSI_CAMERA_USE_NO_COMPRESSION);
	frame->AddConsumer(camera_p, cam_write, "1");

	VideoPainter *camera_plot = ssi_create_id (VideoPainter, 0, "plot");
	camera_plot->getOptions()->mirror = false;
	camera_plot->getOptions()->flip = true;
	camera_plot->getOptions()->setTitle("camera");
	frame->AddConsumer(camera_p, camera_plot, "1");

	decorator->add("console", 0, 0, 650, 800);
	decorator->add("plot*", 650, 0, 400, 800);

	frame->Start();
	frame->Wait();
	frame->Stop();
	frame->Clear();
}

bool ex_camfile (void *args) {		

	const ssi_char_t *filename = "video.avi";	

	if (!ssi_exists (filename)) {
		record ();		
	}

	ITheFramework *frame = Factory::GetFramework ();

	Decorator *decorator = ssi_create (Decorator, 0, true);
	frame->AddDecorator(decorator);

	CameraReader *cam = ssi_create (CameraReader, 0, true);
	cam->getOptions()->setPath(filename);
	ITransformable *cam_p  = frame->AddProvider(cam, SSI_CAMERAREADER_PROVIDER_NAME, 0, "2.0s");
	frame->AddSensor(cam);

	VideoPainter *cam_plot = ssi_create_id (VideoPainter, 0, "plot");
	cam_plot->getOptions()->setTitle("video");		
	//cam_plot->getOptions()->mirror = false;
	//cam_plot->getOptions()->flip = true;
	frame->AddConsumer(cam_p, cam_plot, "1");
	
	decorator->add("console", 0, 0, 650, 800);
	decorator->add("plot*", 650, 0, 400, 400);
	decorator->add("monitor*", 650, 400, 400, 400);

	frame->Start();
	cam->wait();
	frame->Stop();
	frame->Clear();

	return true;
}		

bool ex_camscreen (void *args) {

	ITheFramework *frame = Factory::GetFramework ();

	Decorator *decorator = ssi_create (Decorator, 0, true);
	frame->AddDecorator(decorator);

	Audio *audio = ssi_create (Audio, "audio", true);
	ITransformable *audio_p = frame->AddProvider(audio, SSI_AUDIO_PROVIDER_NAME);
	frame->AddSensor(audio);
	
	CameraScreen *screen = ssi_create (CameraScreen, "screen", true);
	screen->getOptions()->mouse = true;
	screen->getOptions()->mouse_size = 10;
	screen->getOptions()->setResize(640, 480);
	screen->getOptions()->fps = 10.0;
	screen->getOptions()->setRegion(100, 100, 800, 600);
	ITransformable *screen_p = frame->AddProvider(screen, SSI_CAMERASCREEN_PROVIDER_NAME, 0, "2.0s");
	frame->AddSensor(screen);

	VideoPainter *cam_plot = ssi_create_id (VideoPainter, 0, "plot");
	cam_plot->getOptions()->setTitle("screen");
	ssi_video_params_t screen_format = screen->getFormat();
	frame->AddConsumer(screen_p, cam_plot, "1");

	ITransformable *ts[] = { screen_p, audio_p };
	CameraWriter *cam_write = ssi_create (CameraWriter, 0, true);
	cam_write->getOptions()->setPath("screen.avi");
	cam_write->getOptions()->setCompression("Microsoft Video 1");
	//cam_write->getOptions()->setCompression(SSI_CAMERA_USE_NO_COMPRESSION);
	frame->AddConsumer(2, ts, cam_write, "1");

	decorator->add("console", 0, 0, 650, 800);
	decorator->add("plot*", 650, 0, 400, 400);
	decorator->add("monitor*", 650, 400, 400, 400);

	frame->Start();
	frame->Wait();
	frame->Stop();
	frame->Clear();

	return true;
}