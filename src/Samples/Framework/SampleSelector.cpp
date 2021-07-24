#define _CRT_SECURE_NO_WARNINGS
#include "SampleSelector.h"
#include <iostream>

#include "Samples/Chapter06Sample01.h"
#include "Samples/Chapter08Sample01.h"
#include "Samples/Chapter09Sample01.h"
#include "Samples/Chapter10Sample01.h"
#include "Samples/Chapter10Sample02.h"
#include "Samples/Chapter11Sample01.h"
#include "Samples/Chapter11Sample02.h"
#include "Samples/Chapter11Sample03.h"
#include "Samples/Chapter12Sample01.h"
#include "Samples/Chapter12Sample02.h"
#include "Samples/Chapter12Sample03.h"
#include "Samples/Chapter13Sample01.h"
#include "Samples/Chapter13Sample02.h"
#include "Samples/Chapter13Sample03.h"
#include "Samples/Chapter14Sample01.h"
#include "Samples/Chapter15Sample01.h"

SampleSelector::SampleSelector() {
	mCurrentSample = 0;
	mIsRunning = false;
}

void SampleSelector::ImGui(nk_context* context, float xPos) {
	if (mCurrentSample != 0) {
		nk_begin(context, "Sample Exit", nk_rect(xPos, 175.0f + 125.0f, 200.0f, 33.0f), NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR);
		nk_layout_row_static(context, 20, 180, 1);

		if (nk_button_label(context, "Close Current Sample")) {
			mCurrentSample->Shutdown();
			delete mCurrentSample;
			mCurrentSample = 0;
		}

		nk_end(context);
	}
	else {
		nk_begin(context, "Sample Selector", nk_rect(5.0f, 5.0f, 200.0f, 500.0f), NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR);
		nk_layout_row_static(context, 25, 190, 1);

		nk_label(context, "Select a Sample to Load", NK_TEXT_LEFT);
		if (nk_button_label(context, "RHI Demo")) {
			mCurrentSample = new Chapter06Sample01();
			mCurrentSample->Initialize();
		}
		if (nk_button_label(context, "Ccurves")) {
			mCurrentSample = new Chapter08Sample01();
			mCurrentSample->Initialize();
		}
		if (nk_button_label(context, "Animation Clips")) {
			mCurrentSample = new Chapter09Sample01();
			mCurrentSample->Initialize();
		}
		if (nk_button_label(context, "Bind Pose")) {
			mCurrentSample = new Chapter10Sample01();
			mCurrentSample->Initialize();
		}
		if (nk_button_label(context, "Mesh Shinning")) {
			mCurrentSample = new Chapter10Sample02();
			mCurrentSample->Initialize();
		}
		if (nk_button_label(context, "Pre-generated skin matrix")) {
			mCurrentSample = new Chapter11Sample01();
			mCurrentSample->Initialize();
		}
		if (nk_button_label(context, "Faster Sampling")) {
			mCurrentSample = new Chapter11Sample02();
			mCurrentSample->Initialize();
		}
		if (nk_button_label(context, "Pose palette")) {
			mCurrentSample = new Chapter11Sample03();
			mCurrentSample->Initialize();
		}
		if (nk_button_label(context, "Blending")) {
			mCurrentSample = new Chapter12Sample01();
			mCurrentSample->Initialize();
		}
		if (nk_button_label(context, "Crossfading")) {
			mCurrentSample = new Chapter12Sample02();
			mCurrentSample->Initialize();
		}
		if (nk_button_label(context, "Additive Blending")) {
			mCurrentSample = new Chapter12Sample03();
			mCurrentSample->Initialize();
		}
		if (nk_button_label(context, "CCDIK")) {
			mCurrentSample = new Chapter13Sample01();
			mCurrentSample->Initialize();
		}
		if (nk_button_label(context, "FABRIK")) {
			mCurrentSample = new Chapter13Sample02();
			mCurrentSample->Initialize();
		}
		if (nk_button_label(context, "Leg IK")) {
			mCurrentSample = new Chapter13Sample03();
			mCurrentSample->Initialize();
		}
		if (nk_button_label(context, "LBS vs DQS")) {
			mCurrentSample = new Chapter14Sample01();
			mCurrentSample->Initialize();
		}
		if (nk_button_label(context, "Crowds")) {
			mCurrentSample = new Chapter15Sample01();
			mCurrentSample->Initialize();
		}

		nk_end(context);
	}
}

SampleSelector::~SampleSelector() {
	if (mIsRunning) {
		std::cout << "Error, sample selector is still running";
	}
	if (mCurrentSample != 0) {
		std::cout << "Error sample still exists\n";
	}
}

Application* SampleSelector::GetCurrentSample() {
	return mCurrentSample;
}

void SampleSelector::Initialize() {
	if (mIsRunning) {
		std::cout << "Starting sample selector multiple times, might leak memory and resources\n";
	}
	mIsRunning = true;
	mCurrentSample = 0;
}

void SampleSelector::Shutdown() {
	if (!mIsRunning) {
		std::cout << "Shutting down inactive sample selector\n";
	}
	if (mCurrentSample != 0) {
		mCurrentSample->Shutdown();
		delete mCurrentSample;
	}
	mCurrentSample = 0;
	mIsRunning = false;
}

bool SampleSelector::IsRunning() {
	return mIsRunning;
}