#pragma once

#include <msclr/marshal_cppstd.h>
#include <iostream>

#include "libusb.h"

#include "sensor_ctl_301.h"
#include "winusb_ctl.h"
#include "libusb_ctl.h"

#include "sensor_init.h"

static uint8_t GAIN = NULL;
static uint8_t VRT = NULL;
static uint8_t VRB = NULL;
static uint8_t DCOFFSET = NULL;

static uint8_t wet_GAIN = NULL;
static uint8_t wet_VRT = NULL;
static uint8_t wet_VRB = NULL;
static uint8_t wet_DCOFFSET = NULL;

static boolean cali_step1;
static boolean cali_step2;
static boolean cali_step3;

static INT sensorType;

static WINUSB_DEVICE_DATA	winusbDeviceData;
static LIBUSB_DEVICE_DATA	libusbDeviceData;
static PIPE_ID				PipeID;

namespace FingerModuleCalibration {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Drawing::Imaging;
	using namespace System::Text;
	using namespace System::IO;

	using namespace System::Diagnostics;
	using namespace System::Runtime::InteropServices;
	using namespace System::Threading;
	
	/// <summary>
	/// GUI 的摘要
	/// </summary>
	public ref class GUI : public System::Windows::Forms::Form
	{
	public:
		GUI(void)
		{
			InitializeComponent();
			//
			//TODO:  在此加入建構函式程式碼
			//
		}

	protected:
		/// <summary>
		/// 清除任何使用中的資源。
		/// </summary>
		~GUI()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label4;
	protected:
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::TextBox^  tb_value;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::TextBox^  tb_register;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  btnRead_Reg;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::PictureBox^  pb_image;
	private: System::Windows::Forms::Label^  lb_info;
	private: System::Windows::Forms::Button^  btnRealTime;
	private: System::ComponentModel::IContainer^  components;
	private: System::Windows::Forms::Timer^  realtime_timer;
	private: System::Windows::Forms::Button^  btnWrite_Reg;
	private: System::Windows::Forms::Button^  btnOpen_device;
	private: System::Windows::Forms::Timer^  swipe_cali_timer;
	private: System::Windows::Forms::Label^  label_E0;
	private: System::Windows::Forms::Label^  label_E1;
	private: System::Windows::Forms::Label^  label_E2;
	private: System::Windows::Forms::Label^  label_E6;
	private: System::Windows::Forms::Button^  btnSwipeModelCalibration;
	private: System::Windows::Forms::CheckBox^  cbWinUSB;
	private: System::Windows::Forms::CheckBox^  cbLibUSB;


	private:
		/// <summary>
		/// 設計工具所需的變數。
		/// </summary>
		Bitmap^ logo_bmp;

		MemoryStream^ memStream;
		UCHAR frame_toggle = 0x02;
		array<Byte>^ byteArray;

		USHORT	pixel_cnt = 0;
		INT		transfer_cnt = 0;

		BOOL choiceUSBType = TRUE;


#pragma region Windows Form Designer generated code
		/// <summary>
		/// 此為設計工具支援所需的方法 - 請勿使用程式碼編輯器
		/// 修改這個方法的內容。
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->tb_value = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->tb_register = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->btnRead_Reg = (gcnew System::Windows::Forms::Button());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->pb_image = (gcnew System::Windows::Forms::PictureBox());
			this->lb_info = (gcnew System::Windows::Forms::Label());
			this->btnRealTime = (gcnew System::Windows::Forms::Button());
			this->realtime_timer = (gcnew System::Windows::Forms::Timer(this->components));
			this->btnWrite_Reg = (gcnew System::Windows::Forms::Button());
			this->btnOpen_device = (gcnew System::Windows::Forms::Button());
			this->swipe_cali_timer = (gcnew System::Windows::Forms::Timer(this->components));
			this->label_E0 = (gcnew System::Windows::Forms::Label());
			this->label_E1 = (gcnew System::Windows::Forms::Label());
			this->label_E2 = (gcnew System::Windows::Forms::Label());
			this->label_E6 = (gcnew System::Windows::Forms::Label());
			this->btnSwipeModelCalibration = (gcnew System::Windows::Forms::Button());
			this->cbWinUSB = (gcnew System::Windows::Forms::CheckBox());
			this->cbLibUSB = (gcnew System::Windows::Forms::CheckBox());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pb_image))->BeginInit();
			this->SuspendLayout();
			// 
			// label4
			// 
			this->label4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label4->Location = System::Drawing::Point(213, 145);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(283, 18);
			this->label4->TabIndex = 25;
			this->label4->Text = L"VID : 0x0483,   PID : 0x0040";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->tb_value);
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->tb_register);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->groupBox1->Location = System::Drawing::Point(504, 140);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(181, 115);
			this->groupBox1->TabIndex = 24;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Register Parameter";
			// 
			// tb_value
			// 
			this->tb_value->Enabled = false;
			this->tb_value->Location = System::Drawing::Point(90, 84);
			this->tb_value->MaxLength = 2;
			this->tb_value->Name = L"tb_value";
			this->tb_value->Size = System::Drawing::Size(68, 24);
			this->tb_value->TabIndex = 4;
			this->tb_value->Text = L"00";
			this->tb_value->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(122, 86);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(0, 18);
			this->label3->TabIndex = 3;
			// 
			// tb_register
			// 
			this->tb_register->Enabled = false;
			this->tb_register->Location = System::Drawing::Point(90, 41);
			this->tb_register->MaxLength = 2;
			this->tb_register->Name = L"tb_register";
			this->tb_register->Size = System::Drawing::Size(68, 24);
			this->tb_register->TabIndex = 2;
			this->tb_register->Text = L"00";
			this->tb_register->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(7, 86);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(70, 20);
			this->label2->TabIndex = 1;
			this->label2->Text = L"Value    :";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(7, 43);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(77, 20);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Register :";
			// 
			// btnRead_Reg
			// 
			this->btnRead_Reg->BackColor = System::Drawing::SystemColors::Control;
			this->btnRead_Reg->Enabled = false;
			this->btnRead_Reg->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->btnRead_Reg->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnRead_Reg->Location = System::Drawing::Point(360, 272);
			this->btnRead_Reg->Name = L"btnRead_Reg";
			this->btnRead_Reg->Size = System::Drawing::Size(138, 40);
			this->btnRead_Reg->TabIndex = 21;
			this->btnRead_Reg->Text = L"Read Register";
			this->btnRead_Reg->UseVisualStyleBackColor = true;
			this->btnRead_Reg->Click += gcnew System::EventHandler(this, &GUI::btnRead_Register_Click);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->pb_image);
			this->groupBox2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->groupBox2->Location = System::Drawing::Point(12, 12);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(197, 214);
			this->groupBox2->TabIndex = 23;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Show Image";
			// 
			// pb_image
			// 
			this->pb_image->Location = System::Drawing::Point(8, 25);
			this->pb_image->Name = L"pb_image";
			this->pb_image->Size = System::Drawing::Size(180, 180);
			this->pb_image->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pb_image->TabIndex = 0;
			this->pb_image->TabStop = false;
			// 
			// lb_info
			// 
			this->lb_info->BackColor = System::Drawing::Color::White;
			this->lb_info->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->lb_info->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->lb_info->Location = System::Drawing::Point(215, 19);
			this->lb_info->Name = L"lb_info";
			this->lb_info->Size = System::Drawing::Size(470, 110);
			this->lb_info->TabIndex = 22;
			this->lb_info->Text = L"Welcome use usb dongle test ap";
			this->lb_info->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// btnRealTime
			// 
			this->btnRealTime->BackColor = System::Drawing::SystemColors::Control;
			this->btnRealTime->Enabled = false;
			this->btnRealTime->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->btnRealTime->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnRealTime->Location = System::Drawing::Point(215, 272);
			this->btnRealTime->Name = L"btnRealTime";
			this->btnRealTime->Size = System::Drawing::Size(138, 40);
			this->btnRealTime->TabIndex = 19;
			this->btnRealTime->Text = L"RealTime";
			this->btnRealTime->UseVisualStyleBackColor = true;
			this->btnRealTime->Click += gcnew System::EventHandler(this, &GUI::btnRealTime_Click);
			// 
			// realtime_timer
			// 
			this->realtime_timer->Tick += gcnew System::EventHandler(this, &GUI::realtime_timer_Tick);
			// 
			// btnWrite_Reg
			// 
			this->btnWrite_Reg->BackColor = System::Drawing::SystemColors::Control;
			this->btnWrite_Reg->Enabled = false;
			this->btnWrite_Reg->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->btnWrite_Reg->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnWrite_Reg->Location = System::Drawing::Point(360, 224);
			this->btnWrite_Reg->Name = L"btnWrite_Reg";
			this->btnWrite_Reg->Size = System::Drawing::Size(138, 40);
			this->btnWrite_Reg->TabIndex = 26;
			this->btnWrite_Reg->Text = L"Write Register";
			this->btnWrite_Reg->UseVisualStyleBackColor = true;
			this->btnWrite_Reg->Click += gcnew System::EventHandler(this, &GUI::btnWrite_Reg_Click);
			// 
			// btnOpen_device
			// 
			this->btnOpen_device->BackColor = System::Drawing::SystemColors::Control;
			this->btnOpen_device->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->btnOpen_device->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnOpen_device->Location = System::Drawing::Point(215, 224);
			this->btnOpen_device->Name = L"btnOpen_device";
			this->btnOpen_device->Size = System::Drawing::Size(138, 40);
			this->btnOpen_device->TabIndex = 27;
			this->btnOpen_device->Text = L"Open Device";
			this->btnOpen_device->UseVisualStyleBackColor = true;
			this->btnOpen_device->Click += gcnew System::EventHandler(this, &GUI::btnOpen_device_Click);
			// 
			// swipe_cali_timer
			// 
			this->swipe_cali_timer->Tick += gcnew System::EventHandler(this, &GUI::swipe_cali_timer_Tick);
			// 
			// label_E0
			// 
			this->label_E0->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_E0->Location = System::Drawing::Point(533, 276);
			this->label_E0->Name = L"label_E0";
			this->label_E0->Size = System::Drawing::Size(129, 23);
			this->label_E0->TabIndex = 28;
			this->label_E0->Text = L"E0 :  00";
			// 
			// label_E1
			// 
			this->label_E1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_E1->Location = System::Drawing::Point(533, 315);
			this->label_E1->Name = L"label_E1";
			this->label_E1->Size = System::Drawing::Size(129, 23);
			this->label_E1->TabIndex = 29;
			this->label_E1->Text = L"E1 :  00";
			// 
			// label_E2
			// 
			this->label_E2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_E2->Location = System::Drawing::Point(533, 354);
			this->label_E2->Name = L"label_E2";
			this->label_E2->Size = System::Drawing::Size(129, 23);
			this->label_E2->TabIndex = 30;
			this->label_E2->Text = L"E2 :  00";
			// 
			// label_E6
			// 
			this->label_E6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_E6->Location = System::Drawing::Point(533, 393);
			this->label_E6->Name = L"label_E6";
			this->label_E6->Size = System::Drawing::Size(129, 23);
			this->label_E6->TabIndex = 31;
			this->label_E6->Text = L"E6 :  00";
			// 
			// btnSwipeModelCalibration
			// 
			this->btnSwipeModelCalibration->BackColor = System::Drawing::SystemColors::Control;
			this->btnSwipeModelCalibration->Enabled = false;
			this->btnSwipeModelCalibration->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->btnSwipeModelCalibration->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(136)));
			this->btnSwipeModelCalibration->Location = System::Drawing::Point(216, 354);
			this->btnSwipeModelCalibration->Name = L"btnSwipeModelCalibration";
			this->btnSwipeModelCalibration->Size = System::Drawing::Size(283, 40);
			this->btnSwipeModelCalibration->TabIndex = 32;
			this->btnSwipeModelCalibration->Text = L"Swipe Calibration";
			this->btnSwipeModelCalibration->UseVisualStyleBackColor = true;
			this->btnSwipeModelCalibration->Click += gcnew System::EventHandler(this, &GUI::btnSwipeModelCalibration_Click);
			// 
			// cbWinUSB
			// 
			this->cbWinUSB->Checked = true;
			this->cbWinUSB->CheckState = System::Windows::Forms::CheckState::Checked;
			this->cbWinUSB->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->cbWinUSB->Location = System::Drawing::Point(238, 175);
			this->cbWinUSB->Name = L"cbWinUSB";
			this->cbWinUSB->Size = System::Drawing::Size(115, 33);
			this->cbWinUSB->TabIndex = 33;
			this->cbWinUSB->Text = L"WinUSB";
			this->cbWinUSB->UseVisualStyleBackColor = true;
			this->cbWinUSB->CheckedChanged += gcnew System::EventHandler(this, &GUI::WinUSB_CheckChanged);
			// 
			// cbLibUSB
			// 
			this->cbLibUSB->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->cbLibUSB->Location = System::Drawing::Point(381, 175);
			this->cbLibUSB->Name = L"cbLibUSB";
			this->cbLibUSB->Size = System::Drawing::Size(115, 33);
			this->cbLibUSB->TabIndex = 34;
			this->cbLibUSB->Text = L"LibUSB";
			this->cbLibUSB->UseVisualStyleBackColor = true;
			this->cbLibUSB->CheckedChanged += gcnew System::EventHandler(this, &GUI::LibUSB_CheckedChanged);
			// 
			// GUI
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(697, 420);
			this->Controls->Add(this->cbLibUSB);
			this->Controls->Add(this->cbWinUSB);
			this->Controls->Add(this->btnSwipeModelCalibration);
			this->Controls->Add(this->label_E6);
			this->Controls->Add(this->label_E2);
			this->Controls->Add(this->label_E1);
			this->Controls->Add(this->label_E0);
			this->Controls->Add(this->btnOpen_device);
			this->Controls->Add(this->btnWrite_Reg);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->btnRead_Reg);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->lb_info);
			this->Controls->Add(this->btnRealTime);
			this->Name = L"GUI";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"FingerModuleCalibration";
			this->Load += gcnew System::EventHandler(this, &GUI::GUI_Load);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pb_image))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void GUI_Load(System::Object^  sender, System::EventArgs^  e) {
				 
				 cbWinUSB->Checked = TRUE;
				 choiceUSBType = TRUE;

				 lb_info->ForeColor = Color::Black;
				 lb_info->Text = "Welcome use usb dongle test ap,\n\n choose WinUSB or LibUSB to communicate.";
	}

	private: System::Void btnOpen_device_Click(System::Object^  sender, System::EventArgs^  e) {
				 HRESULT hr = S_OK;

				 BOOL	 noDevice;
				 BOOL	 bResult = TRUE;

				 UCHAR	 sensorId;

				 if (choiceUSBType) {
					 hr = WinUSB_OpenDevice(&winusbDeviceData, &noDevice);
				 }
				 else {
					 hr = LibUSB_OpenDevice(&libusbDeviceData,  0x0483, 0x0040);
				 }

				 if (SUCCEEDED(hr)) {
					 lb_info->ForeColor = Color::Black; 
								
					 if (choiceUSBType) {
						 lb_info->Text = "Open device success. (WinUSB)\n";
						 bResult = WinUsb_ReadRegister(winusbDeviceData.WinusbHandle, 0xFF, &sensorId);
					 }
					 else {
						 lb_info->Text = "Open device success. (LibUSB)\n";
						 bResult = LibUSB_ReadRegister(libusbDeviceData.LibusbHandle, 0xFF, &sensorId);
					 }

					 if (bResult) {
						 if (sensorId == _301_ID || sensorId == _301_ID2 || sensorId == _301_ID3 || sensorId == _301_ID4 || sensorId == _301_ID5 || sensorId == _301_ID6) {
							 sensorType = 301;
							 lb_info->Text += "\nSensorType: " + sensorType;
							 if (choiceUSBType) {
								 bResult = WinUSB_WriteRegister(winusbDeviceData.WinusbHandle, _301_regs_table, ARRAYSIZE(_301_regs_table));
								 bResult = WinUSB_WriteRegister(winusbDeviceData.WinusbHandle, _301_touch_init_table, ARRAYSIZE(_301_touch_init_table));
							 }
							 else {
								 bResult = LibUSB_WriteRegister(libusbDeviceData.LibusbHandle, _301_regs_table, ARRAYSIZE(_301_regs_table));
								 bResult = LibUSB_WriteRegister(libusbDeviceData.LibusbHandle, _301_touch_init_table, ARRAYSIZE(_301_touch_init_table));
							 }
							 switch_button_state(true);
							 btnOpen_device->Enabled = false;
						 }
						 else if (sensorId == _305_ID || sensorId == _305_ID2 || sensorId == _305_ID3) {
							 sensorType = 305;
							 lb_info->Text += "\nSensorType: " + sensorType;
							 if (choiceUSBType) {
								 bResult = WinUSB_WriteRegister(winusbDeviceData.WinusbHandle, _305_touch_init_table, ARRAYSIZE(_305_touch_init_table));
							 }
							 else {
								 bResult = LibUSB_WriteRegister(libusbDeviceData.LibusbHandle, _305_touch_init_table, ARRAYSIZE(_305_touch_init_table));
							 }
							 switch_button_state(true);
							 btnOpen_device->Enabled = false;
						 }
						 else {
							 lb_info->Text += "\nSensorType: Unknown" ;
						 }
					 }
				 }
				 else {
					 printf(" Open device fail.\n");
					 lb_info->ForeColor = Color::Red;
					 lb_info->Text = "Device not found,";
					 lb_info->Text += "\n please connect device or verify the driver is installed, ";
					 lb_info->Text += "\n or confirm WinUSB GUID is match.";
				 }
	}

	private: System::Void btnRealTime_Click(System::Object^  sender, System::EventArgs^  e) {
				 int res = -1;
				 BOOL bResult = TRUE;

				 if (btnRealTime->Text == "RealTime") {
					 btnRealTime->Enabled = true;
					 btnRealTime->Text = "Stop";

					 if (choiceUSBType) {
						 if (sensorType == 301) {
							 bResult = WinUSB_WriteRegister(winusbDeviceData.WinusbHandle, _301_regs_table, ARRAYSIZE(_301_regs_table));
							 bResult = WinUSB_WriteRegister(winusbDeviceData.WinusbHandle, _301_touch_init_table, ARRAYSIZE(_301_touch_init_table));
						 }
						 else if (sensorType == 305) {
							 bResult = WinUSB_WriteRegister(winusbDeviceData.WinusbHandle, _305_touch_init_table, ARRAYSIZE(_305_touch_init_table));
						 }
					 }
					 else {
						 if (sensorType == 301) {
							 bResult = LibUSB_WriteRegister(libusbDeviceData.LibusbHandle, _301_regs_table, ARRAYSIZE(_301_regs_table));
							 bResult = LibUSB_WriteRegister(libusbDeviceData.LibusbHandle, _301_touch_init_table, ARRAYSIZE(_301_touch_init_table));
						 }
						 else if (sensorType == 305) {
							 bResult = LibUSB_WriteRegister(libusbDeviceData.LibusbHandle, _305_touch_init_table, ARRAYSIZE(_305_touch_init_table));
						 }
					 }

					 if (bResult) {
						 printf("  Write register success.\n");

						 if (choiceUSBType) {
							 if (sensorType == 301) {
								 bResult = WinUSB_StartStream(winusbDeviceData.WinusbHandle, _301_FRAME_SIZE);
							 }
							 else if (sensorType == 305) {
								 bResult = WinUSB_StartStream(winusbDeviceData.WinusbHandle, _305_FRAME_SIZE);
							 }
						 }
						 else {
							 if (sensorType == 301) {
								 bResult = LibUSB_StartStream(libusbDeviceData.LibusbHandle, _301_FRAME_SIZE);
							 }
							 else if (sensorType == 305) {
								 bResult = LibUSB_StartStream(libusbDeviceData.LibusbHandle, _305_FRAME_SIZE);
							 }
						 }

						 if (bResult) {
							 printf("  Press finger to show real-time image on device\n");
							 lb_info->ForeColor = Color::Black;
							 lb_info->Text = "Press finger to show real-time\n image on device.";
							 lb_info->Refresh();

							 switch_button_state(false);
							 btnRealTime->Enabled = true;

							 realtime_timer->Interval = 33;
							 realtime_timer->Start();

							 memStream = gcnew MemoryStream();
						 }
					 }
					 else {
						 printf("  Write register fail.\n");

						 lb_info->ForeColor = Color::Red;
						 lb_info->Text = "Device not found,\n please connect device.";
						 switch_button_state(false);
						 btnOpen_device->Enabled = true;
					 }

					 /*
					 GAIN = _301_GAIN_INIT;
					 VRT = _301_VRT_INIT;
					 VRB = _301_VRB_INIT;
					 DCOFFSET = _301_DCOFFSET_INIT;

					 //wet_GAIN = _301_WET_GAIN_INIT;
					 //wet_VRT = _301_WET_VRT_INIT;
					 //wet_VRB = _301_WET_VRB_INIT;
					 //wet_DCOFFSET = _301_WET_DCOFFSET_INIT;

					 printf("Real Time Register -> %02x, %02x, %02x, %02x\n", GAIN, VRT, VRB, DCOFFSET);

					 uint8_t _301_cali_table[] = {
						 0xE0, GAIN,
						 0xE1, VRT,
						 0xE2, VRB,
						 0xE6, DCOFFSET
					 };

					 if (chipType == 301) {
						 res = set_area_register();

						 res = usb_write_register(_301_cali_table, ARRAYSIZE(_301_cali_table));
					 }
					 else if (chipType == 305) {
						 res = usb_write_register(_305_touch_init_table, ARRAYSIZE(_305_touch_init_table));
					 }

					 if (res < 0) {
						 printf("Init sensor fail.\n");

						 lb_info->ForeColor = Color::Red;
						 lb_info->Text = "Device not found,\n please connect device.";
						 switch_button_state(false);
						 btnOpen_device->Enabled = true;
					 }
					 else {					 
						 printf("Init sensor success. (%d)\n", res);

						 res = check_frame_ready();
				
						 if (res == 1) {
							 cali_step1 = true;
							 cali_step2 = false;

							 res = usb_stop_stream();

							 if (chipType == 301) {
								 res = usb_start_stream(16384);
							 }
							 else if (chipType == 305) {
								 res = usb_start_stream(14400);
							 }

							 if (res < 0) {
								 printf("usb_sensor_start_stream fail. (%d)\n", res);
							 }
							 else {
								 printf("Press finger to show real-time image on device\n");
								 lb_info->ForeColor = Color::Black;
								 lb_info->Text = "Press finger to show real-time\n image on device.";
								 lb_info->Refresh();

								 switch_button_state(false);
								 btnRealTime->Enabled = true;

								 realtime_timer->Interval = 33;
								 realtime_timer->Start();

								 memStream = gcnew MemoryStream();
							 }
						 }
						 else {
							 printf("Check frame ready failed.\n");
						 }
					 }
					 */
				 }
				 else {
					 if (choiceUSBType) {
						 bResult = WinUSB_StopStream(winusbDeviceData.WinusbHandle);
					 }
					 else {
						 bResult = LibUSB_StopStream(libusbDeviceData.LibusbHandle);
					 }

					 if (!bResult) {
						 printf("  xxxUSB_StopStream fail.\n");
					 }
					 else {
						 realtime_timer->Stop();

						 lb_info->ForeColor = Color::Black;
						 lb_info->Text = "Welcome use usb dongle test ap";
						 switch_button_state(true);
						 btnOpen_device->Enabled = false;

						 btnRealTime->Text = "RealTime";
					 }
				 }
	}

	private: System::Void realtime_timer_Tick(System::Object^  sender, System::EventArgs^  e) {
				 int res = -1;

				 BOOL bResult = TRUE;

				 //uint8_t data[TRANSFER_SIZE];
				 UCHAR convert_data[TRANSFER_SIZE];
				 //UCHAR new_data[30000];

				 UCHAR SenMax = 0, SenMin = 0, SenAvg = 0, SenMed = 0;

				 INT dwSum = 0, dwAvg = 0;

				 INT high = 0, low = 0;

				 ULONG transferred = 0;

				 INT tag = 0;
				 //INT dat_len;

				 UINT hdr_idx = 0;
				 UINT dat_idx = 1;

				 int cii = 0;

				 UINT max = 0, min = 0;

				 //int grid[SENSOR_GRID_LEN];

				 //uint8_t* img_data;

				 UCHAR _301_cali_table[] = {
					 0xE0, GAIN,
					 0xE1, VRT,
					 0xE2, VRB,
					 0xE6, DCOFFSET
				 };

				 UCHAR _301_wet_table[] = {
					 0xE0, wet_GAIN,
					 0xE1, wet_VRT,
					 0xE2, wet_VRB,
					 0xE6, wet_DCOFFSET
				 };
				 //printf("Real Time Register -> %02x, %02x, %02x, %02x\n", wet_GAIN, wet_VRT, wet_VRB, wet_DCOFFSET);

				 UCHAR data[4162];

				 if (choiceUSBType) {
					 res = WinUsb_ReadPipe(winusbDeviceData.WinusbHandle, 0x81, data, TRANSFER_SIZE, &transferred, 0);
				 }
				 else {
					 res = libusb_bulk_transfer(libusbDeviceData.LibusbHandle, 0x81, data, TRANSFER_SIZE, (int*)&transferred, 0);
				 }

				 if (res < 0) {
					 printf("  xxxUSB_bulk_transfer error.(%d)\n", res);
				 }
				 else {	
					 printf("transferred (%d)\n", transferred);
					 if (transferred > 0) {
						 if ((data[0] & 0x02) != 0x02) {
							 if ((data[0] & 0x01) != frame_toggle) {

								 frame_toggle = (data[0] & 0x01);

								 array<Byte>^ r = memStream->ToArray();
								 Console::WriteLine("MemoryStream Length = " + r->Length);

								 if ((sensorType == 301) && (r->Length == 16384)) {
									 pb_image->Image = ToGrayBitmap(r, 128, 128);
								 } 
								 else if ((sensorType == 305) && (r->Length == 14400)) {
									 pb_image->Image = ToGrayBitmap(r, 120, 120);
								 }

								 memStream = gcnew MemoryStream();
								 /*
								 if (chipType == 301 && r->Length == 16384) {
									 Marshal::Copy(r, 0, (IntPtr)new_data, r->Length);
 
									 max = min = new_data[0];

									 for (int i = 0; i < r->Length; i++) {
										 if (max < new_data[i]) {
											 max = new_data[i];
										 }
										 else if (min > new_data[i]) {
											 min = new_data[i];
										 }

										 dwSum += new_data[i];
									 }
									 dwAvg = dwSum / r->Length;
									 printf("Max: %d, Min: %d, Avg: %d\n", max, min, dwAvg);

									 for (int i = 4096; i < 4096 + 128; i++) {
										 if (127 <= new_data[i]) {
											 high++;
										 }
										 else {
											 low++;
										 }
									 }

									 for (int i = 8192; i < 8192 + 128; i++) {
										 if (127 <= new_data[i]) {
											 high++;
										 }
										 else {
											 low++;
										 }
									 }

									 for (int i = 12288; i < 12288 + 128; i++) {
										 if (127 <= new_data[i]) {
											 high++;
										 }
										 else {
											 low++;
										 }
									 }
									 printf("High : %d, Low : %d\n", high, low);
									 
									 pb_image->Image = ToGrayBitmap(r, 128, 128);
									 								 
									 if (high < 100) {
										 res = usb_write_register(_301_wet_table, ARRAYSIZE(_301_wet_table));
									 }
									 else {
										 res = usb_write_register(_301_cali_table, ARRAYSIZE(_301_cali_table));
									 }									 
								 }
								 else if (chipType == 305 && r->Length == 14400) {
									 pb_image->Image = ToGrayBitmap(r, 120, 120);
								 }
								 */
							 }

							 for (INT y = 0; y < (INT)transferred; y++) {
								 if (y % 64 != 0) {
									 convert_data[y - tag] = data[y];
								 }
								 else {
									 tag++;
								 }
							 }						 

							 for (INT i = 0; i < (INT)transferred - tag; i++) {
								 convert_data[i] = 255 - convert_data[i];
							 }

							 byteArray = gcnew array<Byte>(transferred - tag);
							 Marshal::Copy((IntPtr)convert_data, byteArray, 0, transferred - tag);

							 memStream->Write(byteArray, 0, transferred - tag);
						 }
					 }
				 }
	}

	private: System::Void btnSwipeModelCalibration_Click(System::Object^  sender, System::EventArgs^  e) {
				 int res = -1;
				 BOOL bResult = TRUE;

				 if (btnSwipeModelCalibration->Text == "Swipe Calibration") {
					 btnSwipeModelCalibration->Enabled = true;
					 btnSwipeModelCalibration->Text = "Stop Calibration..";

					 GAIN = _301_GAIN_INIT;
					 VRT = _301_VRT_INIT;
					 VRB = _301_VRB_INIT;
					 DCOFFSET = _301_DCOFFSET_INIT;

					 bResult = Sensor_Set_SwipeRegister(choiceUSBType);
					 //res = set_swipe_register();

					 if (!bResult) {
						 printf("  Set swipe register fail.\n");
						 lb_info->ForeColor = Color::Red;
						 lb_info->Text = "Device not found,\n please connect device.";
						 switch_button_state(false);
						 btnOpen_device->Enabled = true;
					 }
					 else {
						 printf("  Set swipe register success.\n");

						 bResult = Sensor_CheckFrameReady(choiceUSBType);

						 if (bResult) {
							 cali_step1 = true;
							 cali_step2 = false;
							 cali_step3 = false;

							 if (choiceUSBType) {
								 bResult = WinUSB_StartStream(winusbDeviceData.WinusbHandle, 1024);
							 }
							 else {
								 bResult = LibUSB_StartStream(libusbDeviceData.LibusbHandle, 1024);
							 }

							 if (bResult) {
								 printf("Press finger to calibration register on device.\n");
								 lb_info->ForeColor = Color::Black;
								 lb_info->Text = "Press finger to calibration register\n on device.";
								 lb_info->Refresh();

								 switch_button_state(false);
								 btnSwipeModelCalibration->Enabled = true;

								 swipe_cali_timer->Interval = 33;
								 swipe_cali_timer->Start();

								 memStream = gcnew MemoryStream();
							 }
						 }
					 }
				 }
				 else {
					 if (choiceUSBType) {
						 bResult = WinUSB_StopStream(winusbDeviceData.WinusbHandle);
					 }
					 else {
						 bResult = LibUSB_StopStream(libusbDeviceData.LibusbHandle);
					 }

					 if (bResult) {
						 swipe_cali_timer->Stop();

						 lb_info->ForeColor = Color::Black;
						 lb_info->Text = "Welcome use usb dongle test ap";
						 switch_button_state(true);
						 btnOpen_device->Enabled = false;

						 btnSwipeModelCalibration->Text = "Swipe Calibration";
					 }
				 }
	}

	private: System::Void swipe_cali_timer_Tick(System::Object^  sender, System::EventArgs^  e) {
				 int res = -1;

				 //uint8_t data[TRANSFER_SIZE];
				 //uint8_t convert_data[TRANSFER_SIZE];
				 //uint8_t new_data[30000];

				 uint8_t SenMax = 0, SenMin = 0, SenAvg = 0, SenMed = 0;

				 //uint32_t tmpTouch, tmpSensor;

				 ULONG transferred;
				 int tag = 0;
				 //int dat_len;

				 int hdr_idx = 0;
				 int dat_idx = 1;

				 int max = 0, min = 0;

				 uint8_t* img_data;

				 float intensity = 0.0;

				 uint8_t _301_cali_table[] = {
					 0xE0, GAIN,
					 0xE1, VRT,
					 0xE2, VRB,
					 0xE6, DCOFFSET
				 };

				 BOOL bResult = TRUE;

				 printf("  Swipe Calibration Timer Register -> %02x, %02x, %02x, %02x\n", GAIN, VRT, VRB, DCOFFSET);

				 if (choiceUSBType) {
					 bResult = WinUSB_WriteRegister(winusbDeviceData.WinusbHandle, _301_cali_table, ARRAYSIZE(_301_cali_table));
				 }
				 else {
					 bResult = LibUSB_WriteRegister(libusbDeviceData.LibusbHandle, _301_cali_table, ARRAYSIZE(_301_cali_table));
				 }

				 Sleep(100);

				 if (choiceUSBType) {
					 bResult = WinUSB_GetImage(winusbDeviceData.WinusbHandle, &img_data, &transferred);
				 }
				 else {
					 bResult = LibUSB_GetImage(libusbDeviceData.LibusbHandle, &img_data);
				 }

				 if (bResult) {
					 for (int idx = 0; idx < _301_SWIPE_FRAME_SIZE; idx++) {
						 img_data[idx] = 255 - img_data[idx];
					 }

					 //res = comp_hist(img_data, &SenMax, &SenMin, &SenAvg, &SenMed, 2, 10, 4, 1, 128, 8);
					 //printf("SenMax: %d, SenMin: %d, SenAvg: %d, SenMed: %d\n", SenMax, SenMin, SenAvg, SenMed);

					 //intensity = calc_intensity(img_data, 0, 7, 0, 127, 128);
					 //printf("intensity (%f)\n", intensity);

					 max = min = img_data[0];

					 for (int i = 0; i < _301_SWIPE_FRAME_SIZE; i++) {
						 if (max < img_data[i]) {
							 max = img_data[i];
						 }
						 else if (min > img_data[i]) {
							 min = img_data[i];
						 }
					 }

					 printf("Max: %d, Min: %d\n", max, min);

					 if (cali_step1) {
						 if ((180 < (max & 0xFF)) && (230 > (max & 0xFF)) && (20 < (min & 0xFF)) && (100 > (min & 0xFF))) {
							 cali_step1 = false;
							 cali_step2 = true;
							 printf("Calibration Final Register -> %02x, %02x, %02x, %02x\n", GAIN, VRT, VRB, DCOFFSET);
						 }
						 else {
							 if (DCOFFSET == 0x14) {
								 if (GAIN == 0x00) {
									 res = 100;
									 printf("Error Register 1-> %02x, %02x, %02x, %02x\n", GAIN, VRT, VRB, DCOFFSET);
									 lb_info->ForeColor = Color::Red;
									 lb_info->Text = "Swipe model calibration error.";
									 btnSwipeModelCalibration->Text = "Swipe Calibration";
									 swipe_cali_timer->Stop();
									 switch_button_state(true);
									 btnOpen_device->Enabled = false;
								 }
								 else {
									 GAIN--;
									 DCOFFSET = 0x00;
								 }
							 }
							 else {
								 DCOFFSET++;
							 }
						 }
					 }

					 if (cali_step2) {
						 if (255 == (max & 0xFF)) {
							 cali_step2 = false;
							 cali_step3 = true;
						 }
						 else {
							 if (VRT == 0x00) {
								 printf("Error Register 2-> %02x, %02x, %02x, %02x\n", GAIN, VRT, VRB, DCOFFSET);
								 lb_info->ForeColor = Color::Red;
								 lb_info->Text = "Swipe model calibration error.";
								 btnSwipeModelCalibration->Text = "Swipe Calibration";
								 swipe_cali_timer->Stop();
								 switch_button_state(true);
								 btnOpen_device->Enabled = false;
							 }
							 else {
								 VRT--;
							 }
						 }
					 }

					 if (cali_step3) {
						 if ((255 == (max & 0xFF)) && (0 == (min & 0xFF))) {
							 
							 if (choiceUSBType) {
								 bResult = WinUSB_StopStream(winusbDeviceData.WinusbHandle);
							 }
							 else {
								 bResult = LibUSB_StopStream(libusbDeviceData.LibusbHandle);
							 }

							 printf("Calibration Final Register 3-> %02x, %02x, %02x, %02x\n", GAIN, VRT, VRB, DCOFFSET);

							 lb_info->ForeColor = Color::Black;
							 lb_info->Text = "Swipe Model calibration final.\n";
							 lb_info->Text += "GAIN : " + String::Format("0x{0:X2}", GAIN) + ", VRT : " + String::Format("0x{0:X2}", VRT) + "\n";
							 lb_info->Text += "VRB : " + String::Format("0x{0:X2}", VRB) + ", DCOFFSET : " + String::Format("0x{0:X2}", DCOFFSET);

							 swipe_cali_timer->Stop();
							 btnSwipeModelCalibration->Text = "Swipe Calibration";
							 switch_button_state(true);
							 btnOpen_device->Enabled = false;
						 }
						 else {
							 if (VRB == 0x3F) {
								 printf("Error Register 2-> %02x, %02x, %02x, %02x\n", GAIN, VRT, VRB, DCOFFSET);
								 lb_info->ForeColor = Color::Red;
								 lb_info->Text = "Swipe model calibration error.";
								 btnSwipeModelCalibration->Text = "Swipe Calibration";
								 swipe_cali_timer->Stop();
								 switch_button_state(true);
								 btnOpen_device->Enabled = false;
							 }
							 else {
								 VRB++;
							 }
						 }
					 }

					 label_E0->Text = "E0 : " + String::Format("0x{0:X2}", GAIN);
					 label_E1->Text = "E1 : " + String::Format("0x{0:X2}", VRT);
					 label_E2->Text = "E2 : " + String::Format("0x{0:X2}", VRB);
					 label_E6->Text = "E6 : " + String::Format("0x{0:X2}", DCOFFSET);

					 array<Byte>^ byteArray = gcnew array<Byte>(_301_SWIPE_FRAME_SIZE);
					 Marshal::Copy((IntPtr)img_data, byteArray, 0, _301_SWIPE_FRAME_SIZE);

					 pb_image->Image = ToGrayBitmap(byteArray, 128, 8);
				 }
	}

	
	private: System::Void btnWrite_Reg_Click(System::Object^  sender, System::EventArgs^  e) {
				 ULONG u16_Reg;
				 ULONG u16_Value;
				 UCHAR	write_register[] = { 0x00, 0x00 };
				 BOOL	bResult	= TRUE;

				 if (tb_register->Text != "00" && tb_register->Text != "0" && tb_register->Text != "") {
					 String^ reg = String::Format("0x{0:X2}", tb_register->Text);
					 String^ value = String::Format("0x{0:X2}", tb_value->Text);

					 IntPtr convertReg = Marshal::StringToHGlobalAnsi(reg);
					 char* nativeCharReg = static_cast<char*>(convertReg.ToPointer());
					 u16_Reg = strtol(nativeCharReg, NULL, 16);
					 Console::WriteLine("write reg-> " + u16_Reg);

					 IntPtr convertValue = Marshal::StringToHGlobalAnsi(value);
					 char* nativeCharValue = static_cast<char*>(convertValue.ToPointer());
					 u16_Value = strtol(nativeCharValue, NULL, 16);
					 Console::WriteLine("write value-> " + u16_Value);

					 write_register[0] = (UCHAR)u16_Reg;
					 write_register[1] = (UCHAR)u16_Value;

					 if (choiceUSBType) {
						 bResult = WinUSB_WriteRegister(winusbDeviceData.WinusbHandle, write_register, ARRAYSIZE(write_register));
					 }
					 else {
						 bResult = LibUSB_WriteRegister(libusbDeviceData.LibusbHandle, write_register, ARRAYSIZE(write_register));
					 }

					 if (!bResult) {
						 printf("usb_sensor_write_register error.\n");
						 lb_info->Text = "Write register fail.\n\n";
						 tb_register->Text = "00";
						 tb_value->Text = "00";
					 }
					 else {
						 lb_info->Text = "Write register success.\n\n";
						 lb_info->Text += "Register address: 0x" + tb_register->Text + ",  Value : 0x" + tb_value->Text;
					 }
				 }
				 else {
					 lb_info->Text = "Write register fail.\n\n";
					 lb_info->Text += "Register address can't be zero or null.";
					 tb_register->Text = "00";
					 tb_value->Text = "00";
				 }
	}

	private: System::Void btnRead_Register_Click(System::Object^  sender, System::EventArgs^  e) { 
				 UCHAR	regData;
				 ULONG  u16;
				 BOOL	bResult = TRUE;

				 if (tb_register->Text != "00" && tb_register->Text != "0" && tb_register->Text != "") {
					 String^ reg = String::Format("0x00{0:X2}", tb_register->Text);

					 IntPtr convertString = Marshal::StringToHGlobalAnsi(reg);
					 char* nativeChar = static_cast<char*>(convertString.ToPointer());

					 u16 = strtol(nativeChar, NULL, 16);

					 if (choiceUSBType) {
						 bResult = WinUsb_ReadRegister(winusbDeviceData.WinusbHandle, (USHORT)u16, &regData);
					 }
					 else {
						 bResult = LibUSB_ReadRegister(libusbDeviceData.LibusbHandle, (USHORT)u16, &regData);
					 }
					 
					 if (!bResult) {
						 printf("usb_read_register error.\n");
						 lb_info->Text = "Read register fail.\n\n";
						 tb_value->Text = "00";
					 }
					 else {
						 tb_value->Text = "" + String::Format("{0:X2}", regData);

						 lb_info->Text = "Read register success.\n\n";
						 lb_info->Text += "Register address: 0x" + tb_register->Text + ",  Value : " + String::Format("0x{0:X2}", regData);
					 }
				 }
				 else {
					 lb_info->Text = "Read register fail.\n\n";
					 lb_info->Text += "Register address can't be zero or null.";
					 tb_register->Text = "00";
					 tb_value->Text = "00";
				 }
	}

	private: System::Void GUI_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
				 BOOL bResult = TRUE;

				 if (choiceUSBType) {
					 bResult = WinUSB_StopStream(winusbDeviceData.WinusbHandle);
					 WinUSB_CloseDevice(&winusbDeviceData);
				 }
				 else {
					 bResult = LibUSB_StopStream(libusbDeviceData.LibusbHandle);
					 LibUSB_CloseDevice(&libusbDeviceData);
				 }
	}

	public: static Bitmap^ ToGrayBitmap(array<Byte>^ rawValues, INT width, INT height) {
				Bitmap^ bmp = gcnew Bitmap(width, height, System::Drawing::Imaging::PixelFormat::Format8bppIndexed);
				System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, bmp->Width, bmp->Height);
				BitmapData^ bmpData = bmp->LockBits(rect, System::Drawing::Imaging::ImageLockMode::WriteOnly, bmp->PixelFormat);

				INT stride = bmpData->Stride;
				INT offset = stride - width;
				IntPtr iptr = bmpData->Scan0;
				INT scanBytes = stride * height;

				INT posScan = 0, posReal = 0;

				array<Byte>^ pixelValues = gcnew array<Byte>(scanBytes);

				for (INT x = 0; x < height; x++) {
					for (INT y = 0; y < width; y++) {
						pixelValues[posScan++] = rawValues[posReal++];
					}
					posScan += offset;
				}

				Marshal::Copy(pixelValues, 0, iptr, scanBytes);
				bmp->UnlockBits(bmpData);

				ColorPalette^ temPalette;

				Bitmap^ tempBmp = gcnew Bitmap(1, 1, System::Drawing::Imaging::PixelFormat::Format8bppIndexed);
				temPalette = tempBmp->Palette;

				for (INT i = 0; i < 256; i++) {
					temPalette->Entries[i] = Color::FromArgb(i, i, i);
				}

				bmp->Palette = temPalette;

				return bmp;
	}

	public: Void switch_button_state(bool flag) {
				btnOpen_device->Enabled = flag;
				btnRealTime->Enabled = flag;
				btnRead_Reg->Enabled = flag;
				btnWrite_Reg->Enabled = flag;
				//btnSwipeModelCalibration->Enabled = flag;

				tb_register->Enabled = flag;
				tb_value->Enabled = flag;
	}

	private: System::Void WinUSB_CheckChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (cbWinUSB->CheckState == CheckState::Checked) {
					 choiceUSBType = TRUE;
					 cbLibUSB->Checked = FALSE;
					 //LibUSB_CloseDevice(&libusbDeviceData);
				 }
				 else if (cbWinUSB->CheckState == CheckState::Unchecked) {
					 choiceUSBType = FALSE;
					 cbLibUSB->Checked = TRUE;
					 WinUSB_CloseDevice(&winusbDeviceData);
				 }
				 switch_button_state(false);
				 btnOpen_device->Enabled = true;
				 lb_info->ForeColor = Color::Black;
				 lb_info->Text = "Welcome use usb dongle test ap,\n\n choose WinUSB or LibUSB to communicate.";
	}

	private: System::Void LibUSB_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (cbLibUSB->CheckState == CheckState::Checked) {
					 choiceUSBType = FALSE;
					 cbWinUSB->Checked = FALSE;
					 WinUSB_CloseDevice(&winusbDeviceData);
				 }
				 else if (cbLibUSB->CheckState == CheckState::Unchecked) {
					 choiceUSBType = TRUE;
					 cbWinUSB->Checked = TRUE;
					 LibUSB_CloseDevice(&libusbDeviceData);
				 }
				 switch_button_state(false);
				 btnOpen_device->Enabled = true;
				 lb_info->ForeColor = Color::Black;
				 lb_info->Text = "Welcome use usb dongle test ap,\n\n choose WinUSB or LibUSB to communicate.";
	}
};
}
