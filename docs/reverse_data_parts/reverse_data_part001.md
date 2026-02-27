<!-- PARTE 1 di ~7 | Caratteri: 368,609 | Bytes: 368,685 -->

using System;
using System.Collections;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.IO.Ports;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Timers;
using System.Windows.Forms;
using DevComponents.DotNetBar;
using DevComponents.DotNetBar.Rendering;
using DevComponents.Editors;
using Microsoft.VisualBasic;
using Microsoft.VisualBasic.CompilerServices;
using Microsoft.VisualBasic.PowerPacks;
using WindowsApplication1.My;

namespace WindowsApplication1;

[DesignerGenerated]
public class KTV2i3oForm : Form
{
	public struct complex
	{
		public double real;

		public double imag;
	}

	public struct SYSREG
	{
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public ushort[] InGain;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public ushort[] OutGain;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 124)]
		public ushort[] EQ_Freq_In;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 124)]
		public ushort[] EQ_Gain_In;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 124)]
		public byte[] EQ_Q_In;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 124)]
		public byte[] EQ_Type_In;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 28)]
		public ushort[] EQ_Freq;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 28)]
		public ushort[] EQ_Gain;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 28)]
		public byte[] EQ_Q;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 28)]
		public byte[] EQ_Type;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
		public byte[] EQ_Bypass;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
		public ushort[] HLPF_Freq;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
		public byte[] HLPF_Type;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
		public ushort[] Fir_Freq;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
		public byte[] Fir_Type;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public ushort[] Delay;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public byte[] Threhold_Noise;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public byte[] ATack_Noise;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public byte[] Decay_Noise;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public byte[] Threhold;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public byte[] ATack;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public byte[] Decay;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public byte[] Mute;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
		public byte[] Phase;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
		public ushort[] MixVol;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
		public byte[] MixRoute;

		public byte mode;

		public byte @lock;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
		public byte[] password;

		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 7)]
		public byte[] bootChar;

		public ushort masterVol;

		public byte AC_V_Sel;

		public byte Clip_TH;

		public byte ver;
	}

	public struct Type_DSPID
	{
		public byte grp;

		public ushort dspID;
	}

	public delegate void SetTextCallback(string text);

	public delegate void InitData(bool ReadWorkMode, bool ModifyKey);

	public delegate void SetMessageFormClose();

	public delegate void DisConnectProcess(string text);

	private IContainer components;

	[AccessedThroughProperty("CenPic")]
	private PictureBox _CenPic;

	[AccessedThroughProperty("SerialPort1")]
	private SerialPort _SerialPort1;

	[AccessedThroughProperty("OpenFileDialog1")]
	private OpenFileDialog _OpenFileDialog1;

	[AccessedThroughProperty("SaveFileDialog1")]
	private SaveFileDialog _SaveFileDialog1;

	[AccessedThroughProperty("IntegerInput1")]
	private IntegerInput _IntegerInput1;

	[AccessedThroughProperty("SuperTabControlPanel1")]
	private SuperTabControlPanel _SuperTabControlPanel1;

	[AccessedThroughProperty("SuperTabControlPanel2")]
	private SuperTabControlPanel _SuperTabControlPanel2;

	[AccessedThroughProperty("tabcontol")]
	private SuperTabControl _tabcontol;

	[AccessedThroughProperty("SuperTabControlPanel9")]
	private SuperTabControlPanel _SuperTabControlPanel9;

	[AccessedThroughProperty("SuperTabControlPanel8")]
	private SuperTabControlPanel _SuperTabControlPanel8;

	[AccessedThroughProperty("SubPic")]
	private PictureBox _SubPic;

	[AccessedThroughProperty("SubLpfSlope")]
	private ComboBox _SubLpfSlope;

	[AccessedThroughProperty("SubHpfSlope")]
	private ComboBox _SubHpfSlope;

	[AccessedThroughProperty("Label500")]
	private Label _Label500;

	[AccessedThroughProperty("Label442")]
	private Label _Label442;

	[AccessedThroughProperty("Label512")]
	private Label _Label512;

	[AccessedThroughProperty("Label452")]
	private Label _Label452;

	[AccessedThroughProperty("Label463")]
	private Label _Label463;

	[AccessedThroughProperty("SubRatioLable")]
	private Label _SubRatioLable;

	[AccessedThroughProperty("Label465")]
	private Label _Label465;

	[AccessedThroughProperty("SubDecayTimeLable")]
	private Label _SubDecayTimeLable;

	[AccessedThroughProperty("SubAttTimeLable")]
	private Label _SubAttTimeLable;

	[AccessedThroughProperty("Label469")]
	private Label _Label469;

	[AccessedThroughProperty("SubMusicDelayLable")]
	private Label _SubMusicDelayLable;

	[AccessedThroughProperty("SubLpfQLable")]
	private Label _SubLpfQLable;

	[AccessedThroughProperty("SubHpfQLable")]
	private Label _SubHpfQLable;

	[AccessedThroughProperty("SubLpfLable")]
	private Label _SubLpfLable;

	[AccessedThroughProperty("SubHpfLable")]
	private Label _SubHpfLable;

	[AccessedThroughProperty("SubThreholdLable")]
	private Label _SubThreholdLable;

	[AccessedThroughProperty("SubRevVolPhase")]
	private Label _SubRevVolPhase;

	[AccessedThroughProperty("SubRevVolLable")]
	private Label _SubRevVolLable;

	[AccessedThroughProperty("SubEchoVolPhase")]
	private Label _SubEchoVolPhase;

	[AccessedThroughProperty("SubEchoVolLable")]
	private Label _SubEchoVolLable;

	[AccessedThroughProperty("SubMicVolPhase")]
	private Label _SubMicVolPhase;

	[AccessedThroughProperty("SubMicVolLable")]
	private Label _SubMicVolLable;

	[AccessedThroughProperty("SubMusicVolPhase")]
	private Label _SubMusicVolPhase;

	[AccessedThroughProperty("SubMusicVolLable")]
	private Label _SubMusicVolLable;

	[AccessedThroughProperty("Label481")]
	private Label _Label481;

	[AccessedThroughProperty("Label482")]
	private Label _Label482;

	[AccessedThroughProperty("Label483")]
	private Label _Label483;

	[AccessedThroughProperty("Label484")]
	private Label _Label484;

	[AccessedThroughProperty("Label488")]
	private Label _Label488;

	[AccessedThroughProperty("Label489")]
	private Label _Label489;

	[AccessedThroughProperty("Label490")]
	private Label _Label490;

	[AccessedThroughProperty("Panel63")]
	private Panel _Panel63;

	[AccessedThroughProperty("SubHS2")]
	private ButtonX _SubHS2;

	[AccessedThroughProperty("SubLS2")]
	private ButtonX _SubLS2;

	[AccessedThroughProperty("SubP2")]
	private ButtonX _SubP2;

	[AccessedThroughProperty("Label492")]
	private Label _Label492;

	[AccessedThroughProperty("Label493")]
	private Label _Label493;

	[AccessedThroughProperty("Label494")]
	private Label _Label494;

	[AccessedThroughProperty("SubFreqEQ2")]
	private DomainUpDown _SubFreqEQ2;

	[AccessedThroughProperty("SubGainEQ2")]
	private DomainUpDown _SubGainEQ2;

	[AccessedThroughProperty("SubQEQ2")]
	private DomainUpDown _SubQEQ2;

	[AccessedThroughProperty("SubTrackBar2")]
	private TrackBar _SubTrackBar2;

	[AccessedThroughProperty("Label495")]
	private Label _Label495;

	[AccessedThroughProperty("Label496")]
	private Label _Label496;

	[AccessedThroughProperty("Panel64")]
	private Panel _Panel64;

	[AccessedThroughProperty("SubHS1")]
	private ButtonX _SubHS1;

	[AccessedThroughProperty("SubLS1")]
	private ButtonX _SubLS1;

	[AccessedThroughProperty("SubP1")]
	private ButtonX _SubP1;

	[AccessedThroughProperty("Label497")]
	private Label _Label497;

	[AccessedThroughProperty("Label498")]
	private Label _Label498;

	[AccessedThroughProperty("Label499")]
	private Label _Label499;

	[AccessedThroughProperty("SubFreqEQ1")]
	private DomainUpDown _SubFreqEQ1;

	[AccessedThroughProperty("SubGainEQ1")]
	private DomainUpDown _SubGainEQ1;

	[AccessedThroughProperty("SubQEQ1")]
	private DomainUpDown _SubQEQ1;

	[AccessedThroughProperty("SubTrackBar1")]
	private TrackBar _SubTrackBar1;

	[AccessedThroughProperty("Label487")]
	private Label _Label487;

	[AccessedThroughProperty("SubLpfQText")]
	private Label _SubLpfQText;

	[AccessedThroughProperty("SubHpfQText")]
	private Label _SubHpfQText;

	[AccessedThroughProperty("Label451")]
	private Label _Label451;

	[AccessedThroughProperty("Label502")]
	private Label _Label502;

	[AccessedThroughProperty("Label503")]
	private Label _Label503;

	[AccessedThroughProperty("Panel65")]
	private Panel _Panel65;

	[AccessedThroughProperty("SubHS3")]
	private ButtonX _SubHS3;

	[AccessedThroughProperty("SubLS3")]
	private ButtonX _SubLS3;

	[AccessedThroughProperty("SubP3")]
	private ButtonX _SubP3;

	[AccessedThroughProperty("Label504")]
	private Label _Label504;

	[AccessedThroughProperty("Label505")]
	private Label _Label505;

	[AccessedThroughProperty("Label506")]
	private Label _Label506;

	[AccessedThroughProperty("SubFreqEQ3")]
	private DomainUpDown _SubFreqEQ3;

	[AccessedThroughProperty("SubGainEQ3")]
	private DomainUpDown _SubGainEQ3;

	[AccessedThroughProperty("SubQEQ3")]
	private DomainUpDown _SubQEQ3;

	[AccessedThroughProperty("SubTrackBar3")]
	private TrackBar _SubTrackBar3;

	[AccessedThroughProperty("Label507")]
	private Label _Label507;

	[AccessedThroughProperty("SubRatio")]
	private TrackBar _SubRatio;

	[AccessedThroughProperty("SubRevVolTrabar")]
	private TrackBar _SubRevVolTrabar;

	[AccessedThroughProperty("SubDecayTime")]
	private TrackBar _SubDecayTime;

	[AccessedThroughProperty("SubEchoVolTrabar")]
	private TrackBar _SubEchoVolTrabar;

	[AccessedThroughProperty("SubLpfQ")]
	private TrackBar _SubLpfQ;

	[AccessedThroughProperty("SubHpfQ")]
	private TrackBar _SubHpfQ;

	[AccessedThroughProperty("SubAttTime")]
	private TrackBar _SubAttTime;

	[AccessedThroughProperty("SubMicVolTrabar")]
	private TrackBar _SubMicVolTrabar;

	[AccessedThroughProperty("SubMusicDelay")]
	private TrackBar _SubMusicDelay;

	[AccessedThroughProperty("SubLpf")]
	private TrackBar _SubLpf;

	[AccessedThroughProperty("SubHpf")]
	private TrackBar _SubHpf;

	[AccessedThroughProperty("SubThrehold")]
	private TrackBar _SubThrehold;

	[AccessedThroughProperty("SubMusicVolTrabar")]
	private TrackBar _SubMusicVolTrabar;

	[AccessedThroughProperty("SuperTabControlPanel7")]
	private SuperTabControlPanel _SuperTabControlPanel7;

	[AccessedThroughProperty("CenHpfSlope")]
	private ComboBox _CenHpfSlope;

	[AccessedThroughProperty("Label417")]
	private Label _Label417;

	[AccessedThroughProperty("Label411")]
	private Label _Label411;

	[AccessedThroughProperty("Label412")]
	private Label _Label412;

	[AccessedThroughProperty("CenRatioLable")]
	private Label _CenRatioLable;

	[AccessedThroughProperty("Label414")]
	private Label _Label414;

	[AccessedThroughProperty("CenDecayTimeLable")]
	private Label _CenDecayTimeLable;

	[AccessedThroughProperty("CenAttTimeLable")]
	private Label _CenAttTimeLable;

	[AccessedThroughProperty("Label419")]
	private Label _Label419;

	[AccessedThroughProperty("CenMusicDelayLable")]
	private Label _CenMusicDelayLable;

	[AccessedThroughProperty("CenHpfLable")]
	private Label _CenHpfLable;

	[AccessedThroughProperty("CenThreholdLable")]
	private Label _CenThreholdLable;

	[AccessedThroughProperty("CenRevVolPhase")]
	private Label _CenRevVolPhase;

	[AccessedThroughProperty("CenRevVolLable")]
	private Label _CenRevVolLable;

	[AccessedThroughProperty("CenEchoVolPhase")]
	private Label _CenEchoVolPhase;

	[AccessedThroughProperty("CenEchoVolLable")]
	private Label _CenEchoVolLable;

	[AccessedThroughProperty("CenMicVolPhase")]
	private Label _CenMicVolPhase;

	[AccessedThroughProperty("CenMicVolLable")]
	private Label _CenMicVolLable;

	[AccessedThroughProperty("CenMusicVolPhase")]
	private Label _CenMusicVolPhase;

	[AccessedThroughProperty("CenMusicVolLable")]
	private Label _CenMusicVolLable;

	[AccessedThroughProperty("Label431")]
	private Label _Label431;

	[AccessedThroughProperty("Label432")]
	private Label _Label432;

	[AccessedThroughProperty("Label433")]
	private Label _Label433;

	[AccessedThroughProperty("Label434")]
	private Label _Label434;

	[AccessedThroughProperty("Panel57")]
	private Panel _Panel57;

	[AccessedThroughProperty("CenHS5")]
	private ButtonX _CenHS5;

	[AccessedThroughProperty("CenLS5")]
	private ButtonX _CenLS5;

	[AccessedThroughProperty("CenP5")]
	private ButtonX _CenP5;

	[AccessedThroughProperty("Label435")]
	private Label _Label435;

	[AccessedThroughProperty("Label436")]
	private Label _Label436;

	[AccessedThroughProperty("Label437")]
	private Label _Label437;

	[AccessedThroughProperty("CenFreqEQ5")]
	private DomainUpDown _CenFreqEQ5;

	[AccessedThroughProperty("CenGainEQ5")]
	private DomainUpDown _CenGainEQ5;

	[AccessedThroughProperty("CenQEQ5")]
	private DomainUpDown _CenQEQ5;

	[AccessedThroughProperty("CenTrackBar5")]
	private TrackBar _CenTrackBar5;

	[AccessedThroughProperty("Label438")]
	private Label _Label438;

	[AccessedThroughProperty("Label439")]
	private Label _Label439;

	[AccessedThroughProperty("Label440")]
	private Label _Label440;

	[AccessedThroughProperty("Panel58")]
	private Panel _Panel58;

	[AccessedThroughProperty("CenHS2")]
	private ButtonX _CenHS2;

	[AccessedThroughProperty("CenLS2")]
	private ButtonX _CenLS2;

	[AccessedThroughProperty("CenP2")]
	private ButtonX _CenP2;

	[AccessedThroughProperty("Label443")]
	private Label _Label443;

	[AccessedThroughProperty("Label444")]
	private Label _Label444;

	[AccessedThroughProperty("Label445")]
	private Label _Label445;

	[AccessedThroughProperty("CenFreqEQ2")]
	private DomainUpDown _CenFreqEQ2;

	[AccessedThroughProperty("CenGainEQ2")]
	private DomainUpDown _CenGainEQ2;

	[AccessedThroughProperty("CenQEQ2")]
	private DomainUpDown _CenQEQ2;

	[AccessedThroughProperty("CenTrackBar2")]
	private TrackBar _CenTrackBar2;

	[AccessedThroughProperty("Label446")]
	private Label _Label446;

	[AccessedThroughProperty("Label447")]
	private Label _Label447;

	[AccessedThroughProperty("Panel59")]
	private Panel _Panel59;

	[AccessedThroughProperty("CenHS1")]
	private ButtonX _CenHS1;

	[AccessedThroughProperty("CenLS1")]
	private ButtonX _CenLS1;

	[AccessedThroughProperty("CenP1")]
	private ButtonX _CenP1;

	[AccessedThroughProperty("Label448")]
	private Label _Label448;

	[AccessedThroughProperty("Label449")]
	private Label _Label449;

	[AccessedThroughProperty("Label450")]
	private Label _Label450;

	[AccessedThroughProperty("CenFreqEQ1")]
	private DomainUpDown _CenFreqEQ1;

	[AccessedThroughProperty("CenGainEQ1")]
	private DomainUpDown _CenGainEQ1;

	[AccessedThroughProperty("CenQEQ1")]
	private DomainUpDown _CenQEQ1;

	[AccessedThroughProperty("CenTrackBar1")]
	private TrackBar _CenTrackBar1;

	[AccessedThroughProperty("Label453")]
	private Label _Label453;

	[AccessedThroughProperty("Label454")]
	private Label _Label454;

	[AccessedThroughProperty("Label455")]
	private Label _Label455;

	[AccessedThroughProperty("Panel60")]
	private Panel _Panel60;

	[AccessedThroughProperty("CenHS3")]
	private ButtonX _CenHS3;

	[AccessedThroughProperty("CenLS3")]
	private ButtonX _CenLS3;

	[AccessedThroughProperty("CenP3")]
	private ButtonX _CenP3;

	[AccessedThroughProperty("Label456")]
	private Label _Label456;

	[AccessedThroughProperty("Label457")]
	private Label _Label457;

	[AccessedThroughProperty("Label458")]
	private Label _Label458;

	[AccessedThroughProperty("CenFreqEQ3")]
	private DomainUpDown _CenFreqEQ3;

	[AccessedThroughProperty("CenGainEQ3")]
	private DomainUpDown _CenGainEQ3;

	[AccessedThroughProperty("CenQEQ3")]
	private DomainUpDown _CenQEQ3;

	[AccessedThroughProperty("CenTrackBar3")]
	private TrackBar _CenTrackBar3;

	[AccessedThroughProperty("Label459")]
	private Label _Label459;

	[AccessedThroughProperty("Panel61")]
	private Panel _Panel61;

	[AccessedThroughProperty("CenHS4")]
	private ButtonX _CenHS4;

	[AccessedThroughProperty("CenLS4")]
	private ButtonX _CenLS4;

	[AccessedThroughProperty("CenP4")]
	private ButtonX _CenP4;

	[AccessedThroughProperty("Label460")]
	private Label _Label460;

	[AccessedThroughProperty("Label461")]
	private Label _Label461;

	[AccessedThroughProperty("Label462")]
	private Label _Label462;

	[AccessedThroughProperty("CenFreqEQ4")]
	private DomainUpDown _CenFreqEQ4;

	[AccessedThroughProperty("CenGainEQ4")]
	private DomainUpDown _CenGainEQ4;

	[AccessedThroughProperty("CenQEQ4")]
	private DomainUpDown _CenQEQ4;

	[AccessedThroughProperty("CenTrackBar4")]
	private TrackBar _CenTrackBar4;

	[AccessedThroughProperty("CenRatio")]
	private TrackBar _CenRatio;

	[AccessedThroughProperty("CenRevVolTrabar")]
	private TrackBar _CenRevVolTrabar;

	[AccessedThroughProperty("CenDecayTime")]
	private TrackBar _CenDecayTime;

	[AccessedThroughProperty("CenEchoVolTrabar")]
	private TrackBar _CenEchoVolTrabar;

	[AccessedThroughProperty("CenAttTime")]
	private TrackBar _CenAttTime;

	[AccessedThroughProperty("CenMicVolTrabar")]
	private TrackBar _CenMicVolTrabar;

	[AccessedThroughProperty("CenHpf")]
	private TrackBar _CenHpf;

	[AccessedThroughProperty("CenThrehold")]
	private TrackBar _CenThrehold;

	[AccessedThroughProperty("CenMusicVolTrabar")]
	private TrackBar _CenMusicVolTrabar;

	[AccessedThroughProperty("SuperTabControlPanel6")]
	private SuperTabControlPanel _SuperTabControlPanel6;

	[AccessedThroughProperty("MainPic")]
	private PictureBox _MainPic;

	[AccessedThroughProperty("Label407")]
	private Label _Label407;

	[AccessedThroughProperty("Label395")]
	private Label _Label395;

	[AccessedThroughProperty("MainRatioLable")]
	private Label _MainRatioLable;

	[AccessedThroughProperty("Label392")]
	private Label _Label392;

	[AccessedThroughProperty("MainDecayTImeLable")]
	private Label _MainDecayTImeLable;

	[AccessedThroughProperty("MainAttTimeLable")]
	private Label _MainAttTimeLable;

	[AccessedThroughProperty("Label359")]
	private Label _Label359;

	[AccessedThroughProperty("MainMusicDelayRLable")]
	private Label _MainMusicDelayRLable;

	[AccessedThroughProperty("MainMusicDelayLLable")]
	private Label _MainMusicDelayLLable;

	[AccessedThroughProperty("MainThreholdLable")]
	private Label _MainThreholdLable;

	[AccessedThroughProperty("MainRevVolPhase")]
	private Label _MainRevVolPhase;

	[AccessedThroughProperty("MainRevVolLable")]
	private Label _MainRevVolLable;

	[AccessedThroughProperty("MainEchoVolPhase")]
	private Label _MainEchoVolPhase;

	[AccessedThroughProperty("MainEchoVolLable")]
	private Label _MainEchoVolLable;

	[AccessedThroughProperty("MainMicVolPhase")]
	private Label _MainMicVolPhase;

	[AccessedThroughProperty("MainMicVolLable")]
	private Label _MainMicVolLable;

	[AccessedThroughProperty("MainMusicVolPhase")]
	private Label _MainMusicVolPhase;

	[AccessedThroughProperty("MainMusicVolLable")]
	private Label _MainMusicVolLable;

	[AccessedThroughProperty("Label360")]
	private Label _Label360;

	[AccessedThroughProperty("Label387")]
	private Label _Label387;

	[AccessedThroughProperty("Label381")]
	private Label _Label381;

	[AccessedThroughProperty("Label361")]
	private Label _Label361;

	[AccessedThroughProperty("Panel52")]
	private Panel _Panel52;

	[AccessedThroughProperty("MainHS5")]
	private ButtonX _MainHS5;

	[AccessedThroughProperty("MainLS5")]
	private ButtonX _MainLS5;

	[AccessedThroughProperty("MainP5")]
	private ButtonX _MainP5;

	[AccessedThroughProperty("Label362")]
	private Label _Label362;

	[AccessedThroughProperty("Label363")]
	private Label _Label363;

	[AccessedThroughProperty("Label364")]
	private Label _Label364;

	[AccessedThroughProperty("MainFreqEQ5")]
	private DomainUpDown _MainFreqEQ5;

	[AccessedThroughProperty("MainGainEQ5")]
	private DomainUpDown _MainGainEQ5;

	[AccessedThroughProperty("MainQEQ5")]
	private DomainUpDown _MainQEQ5;

	[AccessedThroughProperty("MainTrackBar5")]
	private TrackBar _MainTrackBar5;

	[AccessedThroughProperty("Label386")]
	private Label _Label386;

	[AccessedThroughProperty("Label380")]
	private Label _Label380;

	[AccessedThroughProperty("Label365")]
	private Label _Label365;

	[AccessedThroughProperty("Panel53")]
	private Panel _Panel53;

	[AccessedThroughProperty("MainHS2")]
	private ButtonX _MainHS2;

	[AccessedThroughProperty("MainLS2")]
	private ButtonX _MainLS2;

	[AccessedThroughProperty("MainP2")]
	private ButtonX _MainP2;

	[AccessedThroughProperty("Label366")]
	private Label _Label366;

	[AccessedThroughProperty("Label367")]
	private Label _Label367;

	[AccessedThroughProperty("Label368")]
	private Label _Label368;

	[AccessedThroughProperty("MainFreqEQ2")]
	private DomainUpDown _MainFreqEQ2;

	[AccessedThroughProperty("MainGainEQ2")]
	private DomainUpDown _MainGainEQ2;

	[AccessedThroughProperty("MainQEQ2")]
	private DomainUpDown _MainQEQ2;

	[AccessedThroughProperty("MainTrackBar2")]
	private TrackBar _MainTrackBar2;

	[AccessedThroughProperty("Label385")]
	private Label _Label385;

	[AccessedThroughProperty("Label379")]
	private Label _Label379;

	[AccessedThroughProperty("Panel54")]
	private Panel _Panel54;

	[AccessedThroughProperty("MainHS1")]
	private ButtonX _MainHS1;

	[AccessedThroughProperty("MainLS1")]
	private ButtonX _MainLS1;

	[AccessedThroughProperty("MainP1")]
	private ButtonX _MainP1;

	[AccessedThroughProperty("Label369")]
	private Label _Label369;

	[AccessedThroughProperty("Label370")]
	private Label _Label370;

	[AccessedThroughProperty("Label371")]
	private Label _Label371;

	[AccessedThroughProperty("MainFreqEQ1")]
	private DomainUpDown _MainFreqEQ1;

	[AccessedThroughProperty("MainGainEQ1")]
	private DomainUpDown _MainGainEQ1;

	[AccessedThroughProperty("MainQEQ1")]
	private DomainUpDown _MainQEQ1;

	[AccessedThroughProperty("MainTrackBar1")]
	private TrackBar _MainTrackBar1;

	[AccessedThroughProperty("Label406")]
	private Label _Label406;

	[AccessedThroughProperty("Label398")]
	private Label _Label398;

	[AccessedThroughProperty("Label399")]
	private Label _Label399;

	[AccessedThroughProperty("Label396")]
	private Label _Label396;

	[AccessedThroughProperty("Label384")]
	private Label _Label384;

	[AccessedThroughProperty("Panel55")]
	private Panel _Panel55;

	[AccessedThroughProperty("MainHS3")]
	private ButtonX _MainHS3;

	[AccessedThroughProperty("MainLS3")]
	private ButtonX _MainLS3;

	[AccessedThroughProperty("MainP3")]
	private ButtonX _MainP3;

	[AccessedThroughProperty("Label372")]
	private Label _Label372;

	[AccessedThroughProperty("Label373")]
	private Label _Label373;

	[AccessedThroughProperty("Label374")]
	private Label _Label374;

	[AccessedThroughProperty("MainFreqEQ3")]
	private DomainUpDown _MainFreqEQ3;

	[AccessedThroughProperty("MainGainEQ3")]
	private DomainUpDown _MainGainEQ3;

	[AccessedThroughProperty("MainQEQ3")]
	private DomainUpDown _MainQEQ3;

	[AccessedThroughProperty("MainTrackBar3")]
	private TrackBar _MainTrackBar3;

	[AccessedThroughProperty("Label378")]
	private Label _Label378;

	[AccessedThroughProperty("Panel56")]
	private Panel _Panel56;

	[AccessedThroughProperty("MainHS4")]
	private ButtonX _MainHS4;

	[AccessedThroughProperty("MainLS4")]
	private ButtonX _MainLS4;

	[AccessedThroughProperty("MainP4")]
	private ButtonX _MainP4;

	[AccessedThroughProperty("Label375")]
	private Label _Label375;

	[AccessedThroughProperty("Label376")]
	private Label _Label376;

	[AccessedThroughProperty("Label377")]
	private Label _Label377;

	[AccessedThroughProperty("MainFreqEQ4")]
	private DomainUpDown _MainFreqEQ4;

	[AccessedThroughProperty("MainGainEQ4")]
	private DomainUpDown _MainGainEQ4;

	[AccessedThroughProperty("MainQEQ4")]
	private DomainUpDown _MainQEQ4;

	[AccessedThroughProperty("MainTrackBar4")]
	private TrackBar _MainTrackBar4;

	[AccessedThroughProperty("MainRatio")]
	private TrackBar _MainRatio;

	[AccessedThroughProperty("MainDecayTIme")]
	private TrackBar _MainDecayTIme;

	[AccessedThroughProperty("MainRevVolTrabar")]
	private TrackBar _MainRevVolTrabar;

	[AccessedThroughProperty("MainAttTime")]
	private TrackBar _MainAttTime;

	[AccessedThroughProperty("MainEchoVolTrabar")]
	private TrackBar _MainEchoVolTrabar;

	[AccessedThroughProperty("MainMusicDelayR")]
	private TrackBar _MainMusicDelayR;

	[AccessedThroughProperty("MainMusicDelayL")]
	private TrackBar _MainMusicDelayL;

	[AccessedThroughProperty("MainThrehold")]
	private TrackBar _MainThrehold;

	[AccessedThroughProperty("MainMicVolTrabar")]
	private TrackBar _MainMicVolTrabar;

	[AccessedThroughProperty("MainMusicVolTrabar")]
	private TrackBar _MainMusicVolTrabar;

	[AccessedThroughProperty("SuperTabControlPanel10")]
	private SuperTabControlPanel _SuperTabControlPanel10;

	[AccessedThroughProperty("RevPic")]
	private PictureBox _RevPic;

	[AccessedThroughProperty("Label645")]
	private Label _Label645;

	[AccessedThroughProperty("RevTimeLable")]
	private Label _RevTimeLable;

	[AccessedThroughProperty("RevPreDelayLable")]
	private Label _RevPreDelayLable;

	[AccessedThroughProperty("Label652")]
	private Label _Label652;

	[AccessedThroughProperty("Label653")]
	private Label _Label653;

	[AccessedThroughProperty("RevTime")]
	private TrackBar _RevTime;

	[AccessedThroughProperty("RevPreDelay")]
	private TrackBar _RevPreDelay;

	[AccessedThroughProperty("RevLpfLable")]
	private Label _RevLpfLable;

	[AccessedThroughProperty("RevHpfLable")]
	private Label _RevHpfLable;

	[AccessedThroughProperty("Label656")]
	private Label _Label656;

	[AccessedThroughProperty("RevLpf")]
	private TrackBar _RevLpf;

	[AccessedThroughProperty("Label657")]
	private Label _Label657;

	[AccessedThroughProperty("RevHpf")]
	private TrackBar _RevHpf;

	[AccessedThroughProperty("RevDirectVolPhase")]
	private Label _RevDirectVolPhase;

	[AccessedThroughProperty("RevDirectVolLable")]
	private Label _RevDirectVolLable;

	[AccessedThroughProperty("RevVolPhase")]
	private Label _RevVolPhase;

	[AccessedThroughProperty("RevVolLable")]
	private Label _RevVolLable;

	[AccessedThroughProperty("Label662")]
	private Label _Label662;

	[AccessedThroughProperty("Label663")]
	private Label _Label663;

	[AccessedThroughProperty("RevDirectVolTrabar")]
	private TrackBar _RevDirectVolTrabar;

	[AccessedThroughProperty("RevVolTrabar")]
	private TrackBar _RevVolTrabar;

	[AccessedThroughProperty("Label567")]
	private Label _Label567;

	[AccessedThroughProperty("Label568")]
	private Label _Label568;

	[AccessedThroughProperty("Label569")]
	private Label _Label569;

	[AccessedThroughProperty("Panel70")]
	private Panel _Panel70;

	[AccessedThroughProperty("RevHS5")]
	private ButtonX _RevHS5;

	[AccessedThroughProperty("RevLS5")]
	private ButtonX _RevLS5;

	[AccessedThroughProperty("RevP5")]
	private ButtonX _RevP5;

	[AccessedThroughProperty("Label570")]
	private Label _Label570;

	[AccessedThroughProperty("Label571")]
	private Label _Label571;

	[AccessedThroughProperty("Label572")]
	private Label _Label572;

	[AccessedThroughProperty("RevFreqEQ5")]
	private DomainUpDown _RevFreqEQ5;

	[AccessedThroughProperty("RevGainEQ5")]
	private DomainUpDown _RevGainEQ5;

	[AccessedThroughProperty("RevQEQ5")]
	private DomainUpDown _RevQEQ5;

	[AccessedThroughProperty("RevTrackBar5")]
	private TrackBar _RevTrackBar5;

	[AccessedThroughProperty("Label573")]
	private Label _Label573;

	[AccessedThroughProperty("Panel71")]
	private Panel _Panel71;

	[AccessedThroughProperty("RevHS2")]
	private ButtonX _RevHS2;

	[AccessedThroughProperty("RevLS2")]
	private ButtonX _RevLS2;

	[AccessedThroughProperty("RevP2")]
	private ButtonX _RevP2;

	[AccessedThroughProperty("Label574")]
	private Label _Label574;

	[AccessedThroughProperty("Label575")]
	private Label _Label575;

	[AccessedThroughProperty("Label576")]
	private Label _Label576;

	[AccessedThroughProperty("RevFreqEQ2")]
	private DomainUpDown _RevFreqEQ2;

	[AccessedThroughProperty("RevGainEQ2")]
	private DomainUpDown _RevGainEQ2;

	[AccessedThroughProperty("RevQEQ2")]
	private DomainUpDown _RevQEQ2;

	[AccessedThroughProperty("RevTrackBar2")]
	private TrackBar _RevTrackBar2;

	[AccessedThroughProperty("Panel72")]
	private Panel _Panel72;

	[AccessedThroughProperty("RevHS1")]
	private ButtonX _RevHS1;

	[AccessedThroughProperty("RevLS1")]
	private ButtonX _RevLS1;

	[AccessedThroughProperty("RevP1")]
	private ButtonX _RevP1;

	[AccessedThroughProperty("Label577")]
	private Label _Label577;

	[AccessedThroughProperty("Label608")]
	private Label _Label608;

	[AccessedThroughProperty("Label609")]
	private Label _Label609;

	[AccessedThroughProperty("RevFreqEQ1")]
	private DomainUpDown _RevFreqEQ1;

	[AccessedThroughProperty("RevGainEQ1")]
	private DomainUpDown _RevGainEQ1;

	[AccessedThroughProperty("RevQEQ1")]
	private DomainUpDown _RevQEQ1;

	[AccessedThroughProperty("RevTrackBar1")]
	private TrackBar _RevTrackBar1;

	[AccessedThroughProperty("Panel73")]
	private Panel _Panel73;

	[AccessedThroughProperty("RevHS3")]
	private ButtonX _RevHS3;

	[AccessedThroughProperty("RevLS3")]
	private ButtonX _RevLS3;

	[AccessedThroughProperty("RevP3")]
	private ButtonX _RevP3;

	[AccessedThroughProperty("Label610")]
	private Label _Label610;

	[AccessedThroughProperty("Label611")]
	private Label _Label611;

	[AccessedThroughProperty("Label612")]
	private Label _Label612;

	[AccessedThroughProperty("RevFreqEQ3")]
	private DomainUpDown _RevFreqEQ3;

	[AccessedThroughProperty("RevGainEQ3")]
	private DomainUpDown _RevGainEQ3;

	[AccessedThroughProperty("RevQEQ3")]
	private DomainUpDown _RevQEQ3;

	[AccessedThroughProperty("RevTrackBar3")]
	private TrackBar _RevTrackBar3;

	[AccessedThroughProperty("Panel74")]
	private Panel _Panel74;

	[AccessedThroughProperty("RevHS4")]
	private ButtonX _RevHS4;

	[AccessedThroughProperty("RevLS4")]
	private ButtonX _RevLS4;

	[AccessedThroughProperty("RevP4")]
	private ButtonX _RevP4;

	[AccessedThroughProperty("Label613")]
	private Label _Label613;

	[AccessedThroughProperty("Label614")]
	private Label _Label614;

	[AccessedThroughProperty("Label615")]
	private Label _Label615;

	[AccessedThroughProperty("RevFreqEQ4")]
	private DomainUpDown _RevFreqEQ4;

	[AccessedThroughProperty("RevGainEQ4")]
	private DomainUpDown _RevGainEQ4;

	[AccessedThroughProperty("RevQEQ4")]
	private DomainUpDown _RevQEQ4;

	[AccessedThroughProperty("RevTrackBar4")]
	private TrackBar _RevTrackBar4;

	[AccessedThroughProperty("SuperTabControlPanel5")]
	private SuperTabControlPanel _SuperTabControlPanel5;

	[AccessedThroughProperty("EchoPic")]
	private PictureBox _EchoPic;

	[AccessedThroughProperty("EchoDelayRLable")]
	private Label _EchoDelayRLable;

	[AccessedThroughProperty("EchoPreDelayRLable")]
	private Label _EchoPreDelayRLable;

	[AccessedThroughProperty("Label633")]
	private Label _Label633;

	[AccessedThroughProperty("EchoRepeatLable")]
	private Label _EchoRepeatLable;

	[AccessedThroughProperty("EchoDelayLable")]
	private Label _EchoDelayLable;

	[AccessedThroughProperty("EchoPreDelayLable")]
	private Label _EchoPreDelayLable;

	[AccessedThroughProperty("Label641")]
	private Label _Label641;

	[AccessedThroughProperty("Label637")]
	private Label _Label637;

	[AccessedThroughProperty("Label638")]
	private Label _Label638;

	[AccessedThroughProperty("Label639")]
	private Label _Label639;

	[AccessedThroughProperty("Label640")]
	private Label _Label640;

	[AccessedThroughProperty("EchoDelayR")]
	private TrackBar _EchoDelayR;

	[AccessedThroughProperty("EchoPreDelayR")]
	private TrackBar _EchoPreDelayR;

	[AccessedThroughProperty("EchoRepeat")]
	private TrackBar _EchoRepeat;

	[AccessedThroughProperty("EchoDelay")]
	private TrackBar _EchoDelay;

	[AccessedThroughProperty("EchoPreDelay")]
	private TrackBar _EchoPreDelay;

	[AccessedThroughProperty("EchoLpfLable")]
	private Label _EchoLpfLable;

	[AccessedThroughProperty("EchoHpfLable")]
	private Label _EchoHpfLable;

	[AccessedThroughProperty("Label630")]
	private Label _Label630;

	[AccessedThroughProperty("EchoLpf")]
	private TrackBar _EchoLpf;

	[AccessedThroughProperty("Label629")]
	private Label _Label629;

	[AccessedThroughProperty("EchoHpf")]
	private TrackBar _EchoHpf;

	[AccessedThroughProperty("EchoDirectVolPhase")]
	private Label _EchoDirectVolPhase;

	[AccessedThroughProperty("EchoDirectVolLable")]
	private Label _EchoDirectVolLable;

	[AccessedThroughProperty("EchoVolPhase")]
	private Label _EchoVolPhase;

	[AccessedThroughProperty("EchoVolLable")]
	private Label _EchoVolLable;

	[AccessedThroughProperty("Label620")]
	private Label _Label620;

	[AccessedThroughProperty("Label621")]
	private Label _Label621;

	[AccessedThroughProperty("EchoDirectVolTrabar")]
	private TrackBar _EchoDirectVolTrabar;

	[AccessedThroughProperty("EchoVolTrabar")]
	private TrackBar _EchoVolTrabar;

	[AccessedThroughProperty("Label548")]
	private Label _Label548;

	[AccessedThroughProperty("Label549")]
	private Label _Label549;

	[AccessedThroughProperty("Label550")]
	private Label _Label550;

	[AccessedThroughProperty("Panel62")]
	private Panel _Panel62;

	[AccessedThroughProperty("EchoHS5")]
	private ButtonX _EchoHS5;

	[AccessedThroughProperty("EchoLS5")]
	private ButtonX _EchoLS5;

	[AccessedThroughProperty("EchoP5")]
	private ButtonX _EchoP5;

	[AccessedThroughProperty("Label551")]
	private Label _Label551;

	[AccessedThroughProperty("Label552")]
	private Label _Label552;

	[AccessedThroughProperty("Label553")]
	private Label _Label553;

	[AccessedThroughProperty("EchoFreqEQ5")]
	private DomainUpDown _EchoFreqEQ5;

	[AccessedThroughProperty("EchoGainEQ5")]
	private DomainUpDown _EchoGainEQ5;

	[AccessedThroughProperty("EchoQEQ5")]
	private DomainUpDown _EchoQEQ5;

	[AccessedThroughProperty("EchoTrackBar5")]
	private TrackBar _EchoTrackBar5;

	[AccessedThroughProperty("Label554")]
	private Label _Label554;

	[AccessedThroughProperty("Panel66")]
	private Panel _Panel66;

	[AccessedThroughProperty("EchoHS2")]
	private ButtonX _EchoHS2;

	[AccessedThroughProperty("EchoLS2")]
	private ButtonX _EchoLS2;

	[AccessedThroughProperty("EchoP2")]
	private ButtonX _EchoP2;

	[AccessedThroughProperty("Label555")]
	private Label _Label555;

	[AccessedThroughProperty("Label556")]
	private Label _Label556;

	[AccessedThroughProperty("Label557")]
	private Label _Label557;

	[AccessedThroughProperty("EchoFreqEQ2")]
	private DomainUpDown _EchoFreqEQ2;

	[AccessedThroughProperty("EchoGainEQ2")]
	private DomainUpDown _EchoGainEQ2;

	[AccessedThroughProperty("EchoQEQ2")]
	private DomainUpDown _EchoQEQ2;

	[AccessedThroughProperty("EchoTrackBar2")]
	private TrackBar _EchoTrackBar2;

	[AccessedThroughProperty("Panel67")]
	private Panel _Panel67;

	[AccessedThroughProperty("EchoHS1")]
	private ButtonX _EchoHS1;

	[AccessedThroughProperty("EchoLS1")]
	private ButtonX _EchoLS1;

	[AccessedThroughProperty("EchoP1")]
	private ButtonX _EchoP1;

	[AccessedThroughProperty("Label558")]
	private Label _Label558;

	[AccessedThroughProperty("Label559")]
	private Label _Label559;

	[AccessedThroughProperty("Label560")]
	private Label _Label560;

	[AccessedThroughProperty("EchoFreqEQ1")]
	private DomainUpDown _EchoFreqEQ1;

	[AccessedThroughProperty("EchoGainEQ1")]
	private DomainUpDown _EchoGainEQ1;

	[AccessedThroughProperty("EchoQEQ1")]
	private DomainUpDown _EchoQEQ1;

	[AccessedThroughProperty("EchoTrackBar1")]
	private TrackBar _EchoTrackBar1;

	[AccessedThroughProperty("Panel68")]
	private Panel _Panel68;

	[AccessedThroughProperty("EchoHS3")]
	private ButtonX _EchoHS3;

	[AccessedThroughProperty("EchoLS3")]
	private ButtonX _EchoLS3;

	[AccessedThroughProperty("EchoP3")]
	private ButtonX _EchoP3;

	[AccessedThroughProperty("Label561")]
	private Label _Label561;

	[AccessedThroughProperty("Label562")]
	private Label _Label562;

	[AccessedThroughProperty("Label563")]
	private Label _Label563;

	[AccessedThroughProperty("EchoFreqEQ3")]
	private DomainUpDown _EchoFreqEQ3;

	[AccessedThroughProperty("EchoGainEQ3")]
	private DomainUpDown _EchoGainEQ3;

	[AccessedThroughProperty("EchoQEQ3")]
	private DomainUpDown _EchoQEQ3;

	[AccessedThroughProperty("EchoTrackBar3")]
	private TrackBar _EchoTrackBar3;

	[AccessedThroughProperty("Panel69")]
	private Panel _Panel69;

	[AccessedThroughProperty("EchoHS4")]
	private ButtonX _EchoHS4;

	[AccessedThroughProperty("EchoLS4")]
	private ButtonX _EchoLS4;

	[AccessedThroughProperty("EchoP4")]
	private ButtonX _EchoP4;

	[AccessedThroughProperty("Label564")]
	private Label _Label564;

	[AccessedThroughProperty("Label565")]
	private Label _Label565;

	[AccessedThroughProperty("Label566")]
	private Label _Label566;

	[AccessedThroughProperty("EchoFreqEQ4")]
	private DomainUpDown _EchoFreqEQ4;

	[AccessedThroughProperty("EchoGainEQ4")]
	private DomainUpDown _EchoGainEQ4;

	[AccessedThroughProperty("EchoQEQ4")]
	private DomainUpDown _EchoQEQ4;

	[AccessedThroughProperty("EchoTrackBar4")]
	private TrackBar _EchoTrackBar4;

	[AccessedThroughProperty("SuperTabControlPanel4")]
	private SuperTabControlPanel _SuperTabControlPanel4;

	[AccessedThroughProperty("MicBPic")]
	private PictureBox _MicBPic;

	[AccessedThroughProperty("GroupBox38")]
	private GroupBox _GroupBox38;

	[AccessedThroughProperty("NoiseBDecTimeLable")]
	private Label _NoiseBDecTimeLable;

	[AccessedThroughProperty("NoiseBAttTimeLable")]
	private Label _NoiseBAttTimeLable;

	[AccessedThroughProperty("NoiseBThresholdLable")]
	private Label _NoiseBThresholdLable;

	[AccessedThroughProperty("Label273")]
	private Label _Label273;

	[AccessedThroughProperty("Label274")]
	private Label _Label274;

	[AccessedThroughProperty("Label275")]
	private Label _Label275;

	[AccessedThroughProperty("NoiseBDecTime")]
	private TrackBar _NoiseBDecTime;

	[AccessedThroughProperty("NoiseBAttTime")]
	private TrackBar _NoiseBAttTime;

	[AccessedThroughProperty("NoiseBThreshold")]
	private TrackBar _NoiseBThreshold;

	[AccessedThroughProperty("GroupBox39")]
	private GroupBox _GroupBox39;

	[AccessedThroughProperty("MicBRatioLable")]
	private Label _MicBRatioLable;

	[AccessedThroughProperty("MicBDecTimeLable")]
	private Label _MicBDecTimeLable;

	[AccessedThroughProperty("MicBAttTimeLable")]
	private Label _MicBAttTimeLable;

	[AccessedThroughProperty("MicBThresholdLable")]
	private Label _MicBThresholdLable;

	[AccessedThroughProperty("Label280")]
	private Label _Label280;

	[AccessedThroughProperty("Label345")]
	private Label _Label345;

	[AccessedThroughProperty("MicBRatio")]
	private TrackBar _MicBRatio;

	[AccessedThroughProperty("MicBDecTime")]
	private TrackBar _MicBDecTime;

	[AccessedThroughProperty("Label346")]
	private Label _Label346;

	[AccessedThroughProperty("MicBAttTime")]
	private TrackBar _MicBAttTime;

	[AccessedThroughProperty("MicBThreshold")]
	private TrackBar _MicBThreshold;

	[AccessedThroughProperty("Label347")]
	private Label _Label347;

	[AccessedThroughProperty("Label348")]
	private Label _Label348;

	[AccessedThroughProperty("MicB_FBE")]
	private ComboBox _MicB_FBE;

	[AccessedThroughProperty("MicBHpfLable")]
	private Label _MicBHpfLable;

	[AccessedThroughProperty("Label350")]
	private Label _Label350;

	[AccessedThroughProperty("MicBHpf")]
	private TrackBar _MicBHpf;

	[AccessedThroughProperty("Label351")]
	private Label _Label351;

	[AccessedThroughProperty("PanelEx2")]
	private PanelEx _PanelEx2;

	[AccessedThroughProperty("Panel34")]
	private Panel _Panel34;

	[AccessedThroughProperty("Panel37")]
	private Panel _Panel37;

	[AccessedThroughProperty("Panel42")]
	private Panel _Panel42;

	[AccessedThroughProperty("Mic2HS10")]
	private ButtonX _Mic2HS10;

	[AccessedThroughProperty("Mic2LS10")]
	private ButtonX _Mic2LS10;

	[AccessedThroughProperty("Mic2P10")]
	private ButtonX _Mic2P10;

	[AccessedThroughProperty("Label315")]
	private Label _Label315;

	[AccessedThroughProperty("Label316")]
	private Label _Label316;

	[AccessedThroughProperty("Label317")]
	private Label _Label317;

	[AccessedThroughProperty("Mic2FreqEQ10")]
	private DomainUpDown _Mic2FreqEQ10;

	[AccessedThroughProperty("Mic2GainEQ10")]
	private DomainUpDown _Mic2GainEQ10;

	[AccessedThroughProperty("Mic2QEQ10")]
	private DomainUpDown _Mic2QEQ10;

	[AccessedThroughProperty("Mic2TrackBar10")]
	private TrackBar _Mic2TrackBar10;

	[AccessedThroughProperty("Panel43")]
	private Panel _Panel43;

	[AccessedThroughProperty("Mic2HS5")]
	private ButtonX _Mic2HS5;

	[AccessedThroughProperty("Mic2LS5")]
	private ButtonX _Mic2LS5;

	[AccessedThroughProperty("Mic2P5")]
	private ButtonX _Mic2P5;

	[AccessedThroughProperty("Label318")]
	private Label _Label318;

	[AccessedThroughProperty("Label319")]
	private Label _Label319;

	[AccessedThroughProperty("Label320")]
	private Label _Label320;

	[AccessedThroughProperty("Mic2FreqEQ5")]
	private DomainUpDown _Mic2FreqEQ5;

	[AccessedThroughProperty("Mic2GainEQ5")]
	private DomainUpDown _Mic2GainEQ5;

	[AccessedThroughProperty("Mic2QEQ5")]
	private DomainUpDown _Mic2QEQ5;

	[AccessedThroughProperty("Mic2TrackBar5")]
	private TrackBar _Mic2TrackBar5;

	[AccessedThroughProperty("Panel44")]
	private Panel _Panel44;

	[AccessedThroughProperty("Mic2HS2")]
	private ButtonX _Mic2HS2;

	[AccessedThroughProperty("Mic2LS2")]
	private ButtonX _Mic2LS2;

	[AccessedThroughProperty("Mic2P2")]
	private ButtonX _Mic2P2;

	[AccessedThroughProperty("Label321")]
	private Label _Label321;

	[AccessedThroughProperty("Label322")]
	private Label _Label322;

	[AccessedThroughProperty("Label323")]
	private Label _Label323;

	[AccessedThroughProperty("Mic2FreqEQ2")]
	private DomainUpDown _Mic2FreqEQ2;

	[AccessedThroughProperty("Mic2GainEQ2")]
	private DomainUpDown _Mic2GainEQ2;

	[AccessedThroughProperty("Mic2QEQ2")]
	private DomainUpDown _Mic2QEQ2;

	[AccessedThroughProperty("Mic2TrackBar2")]
	private TrackBar _Mic2TrackBar2;

	[AccessedThroughProperty("Panel45")]
	private Panel _Panel45;

	[AccessedThroughProperty("Mic2HS8")]
	private ButtonX _Mic2HS8;

	[AccessedThroughProperty("Mic2LS8")]
	private ButtonX _Mic2LS8;

	[AccessedThroughProperty("Mic2P8")]
	private ButtonX _Mic2P8;

	[AccessedThroughProperty("Label324")]
	private Label _Label324;

	[AccessedThroughProperty("Label325")]
	private Label _Label325;

	[AccessedThroughProperty("Label326")]
	private Label _Label326;

	[AccessedThroughProperty("Mic2FreqEQ8")]
	private DomainUpDown _Mic2FreqEQ8;

	[AccessedThroughProperty("Mic2GainEQ8")]
	private DomainUpDown _Mic2GainEQ8;

	[AccessedThroughProperty("Mic2QEQ8")]
	private DomainUpDown _Mic2QEQ8;

	[AccessedThroughProperty("Mic2TrackBar8")]
	private TrackBar _Mic2TrackBar8;

	[AccessedThroughProperty("Panel46")]
	private Panel _Panel46;

	[AccessedThroughProperty("Mic2HS6")]
	private ButtonX _Mic2HS6;

	[AccessedThroughProperty("Mic2LS6")]
	private ButtonX _Mic2LS6;

	[AccessedThroughProperty("Mic2P6")]
	private ButtonX _Mic2P6;

	[AccessedThroughProperty("Label327")]
	private Label _Label327;

	[AccessedThroughProperty("Label328")]
	private Label _Label328;

	[AccessedThroughProperty("Label329")]
	private Label _Label329;

	[AccessedThroughProperty("Mic2FreqEQ6")]
	private DomainUpDown _Mic2FreqEQ6;

	[AccessedThroughProperty("Mic2GainEQ6")]
	private DomainUpDown _Mic2GainEQ6;

	[AccessedThroughProperty("Mic2QEQ6")]
	private DomainUpDown _Mic2QEQ6;

	[AccessedThroughProperty("Mic2TrackBar6")]
	private TrackBar _Mic2TrackBar6;

	[AccessedThroughProperty("Panel47")]
	private Panel _Panel47;

	[AccessedThroughProperty("Mic2HS1")]
	private ButtonX _Mic2HS1;

	[AccessedThroughProperty("Mic2LS1")]
	private ButtonX _Mic2LS1;

	[AccessedThroughProperty("Mic2P1")]
	private ButtonX _Mic2P1;

	[AccessedThroughProperty("Label330")]
	private Label _Label330;

	[AccessedThroughProperty("Label331")]
	private Label _Label331;

	[AccessedThroughProperty("Label332")]
	private Label _Label332;

	[AccessedThroughProperty("Mic2FreqEQ1")]
	private DomainUpDown _Mic2FreqEQ1;

	[AccessedThroughProperty("Mic2GainEQ1")]
	private DomainUpDown _Mic2GainEQ1;

	[AccessedThroughProperty("Mic2QEQ1")]
	private DomainUpDown _Mic2QEQ1;

	[AccessedThroughProperty("Mic2TrackBar1")]
	private TrackBar _Mic2TrackBar1;

	[AccessedThroughProperty("Panel48")]
	private Panel _Panel48;

	[AccessedThroughProperty("Mic2HS3")]
	private ButtonX _Mic2HS3;

	[AccessedThroughProperty("Mic2LS3")]
	private ButtonX _Mic2LS3;

	[AccessedThroughProperty("Mic2P3")]
	private ButtonX _Mic2P3;

	[AccessedThroughProperty("Label333")]
	private Label _Label333;

	[AccessedThroughProperty("Label334")]
	private Label _Label334;

	[AccessedThroughProperty("Label335")]
	private Label _Label335;

	[AccessedThroughProperty("Mic2FreqEQ3")]
	private DomainUpDown _Mic2FreqEQ3;

	[AccessedThroughProperty("Mic2GainEQ3")]
	private DomainUpDown _Mic2GainEQ3;

	[AccessedThroughProperty("Mic2QEQ3")]
	private DomainUpDown _Mic2QEQ3;

	[AccessedThroughProperty("Mic2TrackBar3")]
	private TrackBar _Mic2TrackBar3;

	[AccessedThroughProperty("Panel49")]
	private Panel _Panel49;

	[AccessedThroughProperty("Mic2HS7")]
	private ButtonX _Mic2HS7;

	[AccessedThroughProperty("Mic2LS7")]
	private ButtonX _Mic2LS7;

	[AccessedThroughProperty("Mic2P7")]
	private ButtonX _Mic2P7;

	[AccessedThroughProperty("Label336")]
	private Label _Label336;

	[AccessedThroughProperty("Label337")]
	private Label _Label337;

	[AccessedThroughProperty("Label338")]
	private Label _Label338;

	[AccessedThroughProperty("Mic2FreqEQ7")]
	private DomainUpDown _Mic2FreqEQ7;

	[AccessedThroughProperty("Mic2GainEQ7")]
	private DomainUpDown _Mic2GainEQ7;

	[AccessedThroughProperty("Mic2QEQ7")]
	private DomainUpDown _Mic2QEQ7;

	[AccessedThroughProperty("Mic2TrackBar7")]
	private TrackBar _Mic2TrackBar7;

	[AccessedThroughProperty("Panel50")]
	private Panel _Panel50;

	[AccessedThroughProperty("Mic2HS9")]
	private ButtonX _Mic2HS9;

	[AccessedThroughProperty("Mic2LS9")]
	private ButtonX _Mic2LS9;

	[AccessedThroughProperty("Mic2P9")]
	private ButtonX _Mic2P9;

	[AccessedThroughProperty("Label339")]
	private Label _Label339;

	[AccessedThroughProperty("Label340")]
	private Label _Label340;

	[AccessedThroughProperty("Label341")]
	private Label _Label341;

	[AccessedThroughProperty("Mic2FreqEQ9")]
	private DomainUpDown _Mic2FreqEQ9;

	[AccessedThroughProperty("Mic2GainEQ9")]
	private DomainUpDown _Mic2GainEQ9;

	[AccessedThroughProperty("Mic2QEQ9")]
	private DomainUpDown _Mic2QEQ9;

	[AccessedThroughProperty("Mic2TrackBar9")]
	private TrackBar _Mic2TrackBar9;

	[AccessedThroughProperty("Panel51")]
	private Panel _Panel51;

	[AccessedThroughProperty("Mic2HS4")]
	private ButtonX _Mic2HS4;

	[AccessedThroughProperty("Mic2LS4")]
	private ButtonX _Mic2LS4;

	[AccessedThroughProperty("Mic2P4")]
	private ButtonX _Mic2P4;

	[AccessedThroughProperty("Label342")]
	private Label _Label342;

	[AccessedThroughProperty("Label343")]
	private Label _Label343;

	[AccessedThroughProperty("Label344")]
	private Label _Label344;

	[AccessedThroughProperty("Mic2FreqEQ4")]
	private DomainUpDown _Mic2FreqEQ4;

	[AccessedThroughProperty("Mic2GainEQ4")]
	private DomainUpDown _Mic2GainEQ4;

	[AccessedThroughProperty("Mic2QEQ4")]
	private DomainUpDown _Mic2QEQ4;

	[AccessedThroughProperty("Mic2TrackBar4")]
	private TrackBar _Mic2TrackBar4;

	[AccessedThroughProperty("Label281")]
	private Label _Label281;

	[AccessedThroughProperty("Label282")]
	private Label _Label282;

	[AccessedThroughProperty("Label284")]
	private Label _Label284;

	[AccessedThroughProperty("Label283")]
	private Label _Label283;

	[AccessedThroughProperty("SuperTabControlPanel3")]
	private SuperTabControlPanel _SuperTabControlPanel3;

	[AccessedThroughProperty("MicAPic")]
	private PictureBox _MicAPic;

	[AccessedThroughProperty("GroupBox40")]
	private GroupBox _GroupBox40;

	[AccessedThroughProperty("NoiseADecTimeLable")]
	private Label _NoiseADecTimeLable;

	[AccessedThroughProperty("NoiseAAttTimeLable")]
	private Label _NoiseAAttTimeLable;

	[AccessedThroughProperty("NoiseAThresholdLable")]
	private Label _NoiseAThresholdLable;

	[AccessedThroughProperty("Label262")]
	private Label _Label262;

	[AccessedThroughProperty("Label263")]
	private Label _Label263;

	[AccessedThroughProperty("Label264")]
	private Label _Label264;

	[AccessedThroughProperty("NoiseADecTime")]
	private TrackBar _NoiseADecTime;

	[AccessedThroughProperty("NoiseAAttTime")]
	private TrackBar _NoiseAAttTime;

	[AccessedThroughProperty("NoiseAThreshold")]
	private TrackBar _NoiseAThreshold;

	[AccessedThroughProperty("GroupBox41")]
	private GroupBox _GroupBox41;

	[AccessedThroughProperty("MicARatioLable")]
	private Label _MicARatioLable;

	[AccessedThroughProperty("MicADecTimeLable")]
	private Label _MicADecTimeLable;

	[AccessedThroughProperty("MicAAttTimeLable")]
	private Label _MicAAttTimeLable;

	[AccessedThroughProperty("MicAThresholdLable")]
	private Label _MicAThresholdLable;

	[AccessedThroughProperty("Label269")]
	private Label _Label269;

	[AccessedThroughProperty("Label352")]
	private Label _Label352;

	[AccessedThroughProperty("MicARatio")]
	private TrackBar _MicARatio;

	[AccessedThroughProperty("MicADecTime")]
	private TrackBar _MicADecTime;

	[AccessedThroughProperty("Label353")]
	private Label _Label353;

	[AccessedThroughProperty("MicAAttTime")]
	private TrackBar _MicAAttTime;

	[AccessedThroughProperty("MicAThreshold")]
	private TrackBar _MicAThreshold;

	[AccessedThroughProperty("Label354")]
	private Label _Label354;

	[AccessedThroughProperty("Label355")]
	private Label _Label355;

	[AccessedThroughProperty("MicA_FBE")]
	private ComboBox _MicA_FBE;

	[AccessedThroughProperty("MicAHpfLable")]
	private Label _MicAHpfLable;

	[AccessedThroughProperty("Label357")]
	private Label _Label357;

	[AccessedThroughProperty("MicAHpf")]
	private TrackBar _MicAHpf;

	[AccessedThroughProperty("Label358")]
	private Label _Label358;

	[AccessedThroughProperty("Label180")]
	private Label _Label180;

	[AccessedThroughProperty("Label182")]
	private Label _Label182;

	[AccessedThroughProperty("Label183")]
	private Label _Label183;

	[AccessedThroughProperty("Label192")]
	private Label _Label192;

	[AccessedThroughProperty("PanelEx1")]
	private PanelEx _PanelEx1;

	[AccessedThroughProperty("MicQEQ16")]
	private DomainUpDown _MicQEQ16;

	[AccessedThroughProperty("Panel27")]
	private Panel _Panel27;

	[AccessedThroughProperty("MicQEQ17")]
	private DomainUpDown _MicQEQ17;

	[AccessedThroughProperty("MicQEQ19")]
	private DomainUpDown _MicQEQ19;

	[AccessedThroughProperty("Panel12")]
	private Panel _Panel12;

	[AccessedThroughProperty("MicHS10")]
	private ButtonX _MicHS10;

	[AccessedThroughProperty("MicLS10")]
	private ButtonX _MicLS10;

	[AccessedThroughProperty("MicP10")]
	private ButtonX _MicP10;

	[AccessedThroughProperty("Label156")]
	private Label _Label156;

	[AccessedThroughProperty("Label161")]
	private Label _Label161;

	[AccessedThroughProperty("Label162")]
	private Label _Label162;

	[AccessedThroughProperty("MicFreqEQ10")]
	private DomainUpDown _MicFreqEQ10;

	[AccessedThroughProperty("MicGainEQ10")]
	private DomainUpDown _MicGainEQ10;

	[AccessedThroughProperty("MicQEQ10")]
	private DomainUpDown _MicQEQ10;

	[AccessedThroughProperty("MicTrackBar10")]
	private TrackBar _MicTrackBar10;

	[AccessedThroughProperty("Panel13")]
	private Panel _Panel13;

	[AccessedThroughProperty("MicHS5")]
	private ButtonX _MicHS5;

	[AccessedThroughProperty("MicLS5")]
	private ButtonX _MicLS5;

	[AccessedThroughProperty("MicP5")]
	private ButtonX _MicP5;

	[AccessedThroughProperty("Label184")]
	private Label _Label184;

	[AccessedThroughProperty("Label185")]
	private Label _Label185;

	[AccessedThroughProperty("Label187")]
	private Label _Label187;

	[AccessedThroughProperty("MicFreqEQ5")]
	private DomainUpDown _MicFreqEQ5;

	[AccessedThroughProperty("MicGainEQ5")]
	private DomainUpDown _MicGainEQ5;

	[AccessedThroughProperty("MicQEQ5")]
	private DomainUpDown _MicQEQ5;

	[AccessedThroughProperty("MicTrackBar5")]
	private TrackBar _MicTrackBar5;

	[AccessedThroughProperty("Panel14")]
	private Panel _Panel14;

	[AccessedThroughProperty("MicHS2")]
	private ButtonX _MicHS2;

	[AccessedThroughProperty("MicLS2")]
	private ButtonX _MicLS2;

	[AccessedThroughProperty("MicP2")]
	private ButtonX _MicP2;

	[AccessedThroughProperty("Label194")]
	private Label _Label194;

	[AccessedThroughProperty("Label205")]
	private Label _Label205;

	[AccessedThroughProperty("Label206")]
	private Label _Label206;

	[AccessedThroughProperty("MicFreqEQ2")]
	private DomainUpDown _MicFreqEQ2;

	[AccessedThroughProperty("MicGainEQ2")]
	private DomainUpDown _MicGainEQ2;

	[AccessedThroughProperty("MicQEQ2")]
	private DomainUpDown _MicQEQ2;

	[AccessedThroughProperty("MicTrackBar2")]
	private TrackBar _MicTrackBar2;

	[AccessedThroughProperty("Panel15")]
	private Panel _Panel15;

	[AccessedThroughProperty("MicHS8")]
	private ButtonX _MicHS8;

	[AccessedThroughProperty("MicLS8")]
	private ButtonX _MicLS8;

	[AccessedThroughProperty("MicP8")]
	private ButtonX _MicP8;

	[AccessedThroughProperty("Label208")]
	private Label _Label208;

	[AccessedThroughProperty("Label209")]
	private Label _Label209;

	[AccessedThroughProperty("Label210")]
	private Label _Label210;

	[AccessedThroughProperty("MicFreqEQ8")]
	private DomainUpDown _MicFreqEQ8;

	[AccessedThroughProperty("MicGainEQ8")]
	private DomainUpDown _MicGainEQ8;

	[AccessedThroughProperty("MicQEQ8")]
	private DomainUpDown _MicQEQ8;

	[AccessedThroughProperty("MicTrackBar8")]
	private TrackBar _MicTrackBar8;

	[AccessedThroughProperty("Panel16")]
	private Panel _Panel16;

	[AccessedThroughProperty("MicHS6")]
	private ButtonX _MicHS6;

	[AccessedThroughProperty("MicLS6")]
	private ButtonX _MicLS6;

	[AccessedThroughProperty("MicP6")]
	private ButtonX _MicP6;

	[AccessedThroughProperty("Label211")]
	private Label _Label211;

	[AccessedThroughProperty("Label212")]
	private Label _Label212;

	[AccessedThroughProperty("Label213")]
	private Label _Label213;

	[AccessedThroughProperty("MicFreqEQ6")]
	private DomainUpDown _MicFreqEQ6;

	[AccessedThroughProperty("MicGainEQ6")]
	private DomainUpDown _MicGainEQ6;

	[AccessedThroughProperty("MicQEQ6")]
	private DomainUpDown _MicQEQ6;

	[AccessedThroughProperty("MicTrackBar6")]
	private TrackBar _MicTrackBar6;

	[AccessedThroughProperty("Panel17")]
	private Panel _Panel17;

	[AccessedThroughProperty("MicHS1")]
	private ButtonX _MicHS1;

	[AccessedThroughProperty("MicLS1")]
	private ButtonX _MicLS1;

	[AccessedThroughProperty("MicP1")]
	private ButtonX _MicP1;

	[AccessedThroughProperty("Label214")]
	private Label _Label214;

	[AccessedThroughProperty("Label215")]
	private Label _Label215;

	[AccessedThroughProperty("Label216")]
	private Label _Label216;

	[AccessedThroughProperty("MicFreqEQ1")]
	private DomainUpDown _MicFreqEQ1;

	[AccessedThroughProperty("MicGainEQ1")]
	private DomainUpDown _MicGainEQ1;

	[AccessedThroughProperty("MicQEQ1")]
	private DomainUpDown _MicQEQ1;

	[AccessedThroughProperty("MicTrackBar1")]
	private TrackBar _MicTrackBar1;

	[AccessedThroughProperty("Panel18")]
	private Panel _Panel18;

	[AccessedThroughProperty("MicHS3")]
	private ButtonX _MicHS3;

	[AccessedThroughProperty("MicLS3")]
	private ButtonX _MicLS3;

	[AccessedThroughProperty("MicP3")]
	private ButtonX _MicP3;

	[AccessedThroughProperty("Label217")]
	private Label _Label217;

	[AccessedThroughProperty("Label218")]
	private Label _Label218;

	[AccessedThroughProperty("Label219")]
	private Label _Label219;

	[AccessedThroughProperty("MicFreqEQ3")]
	private DomainUpDown _MicFreqEQ3;

	[AccessedThroughProperty("MicGainEQ3")]
	private DomainUpDown _MicGainEQ3;

	[AccessedThroughProperty("MicQEQ3")]
	private DomainUpDown _MicQEQ3;

	[AccessedThroughProperty("MicTrackBar3")]
	private TrackBar _MicTrackBar3;

	[AccessedThroughProperty("Panel19")]
	private Panel _Panel19;

	[AccessedThroughProperty("MicHS7")]
	private ButtonX _MicHS7;

	[AccessedThroughProperty("MicLS7")]
	private ButtonX _MicLS7;

	[AccessedThroughProperty("MicP7")]
	private ButtonX _MicP7;

	[AccessedThroughProperty("Label220")]
	private Label _Label220;

	[AccessedThroughProperty("Label221")]
	private Label _Label221;

	[AccessedThroughProperty("Label222")]
	private Label _Label222;

	[AccessedThroughProperty("MicFreqEQ7")]
	private DomainUpDown _MicFreqEQ7;

	[AccessedThroughProperty("MicGainEQ7")]
	private DomainUpDown _MicGainEQ7;

	[AccessedThroughProperty("MicQEQ7")]
	private DomainUpDown _MicQEQ7;

	[AccessedThroughProperty("MicTrackBar7")]
	private TrackBar _MicTrackBar7;

	[AccessedThroughProperty("Panel20")]
	private Panel _Panel20;

	[AccessedThroughProperty("MicHS9")]
	private ButtonX _MicHS9;

	[AccessedThroughProperty("MicLS9")]
	private ButtonX _MicLS9;

	[AccessedThroughProperty("MicP9")]
	private ButtonX _MicP9;

	[AccessedThroughProperty("Label223")]
	private Label _Label223;

	[AccessedThroughProperty("Label224")]
	private Label _Label224;

	[AccessedThroughProperty("Label225")]
	private Label _Label225;

	[AccessedThroughProperty("MicFreqEQ9")]
	private DomainUpDown _MicFreqEQ9;

	[AccessedThroughProperty("MicGainEQ9")]
	private DomainUpDown _MicGainEQ9;

	[AccessedThroughProperty("MicQEQ9")]
	private DomainUpDown _MicQEQ9;

	[AccessedThroughProperty("MicTrackBar9")]
	private TrackBar _MicTrackBar9;

	[AccessedThroughProperty("Panel21")]
	private Panel _Panel21;

	[AccessedThroughProperty("MicHS4")]
	private ButtonX _MicHS4;

	[AccessedThroughProperty("MicLS4")]
	private ButtonX _MicLS4;

	[AccessedThroughProperty("MicP4")]
	private ButtonX _MicP4;

	[AccessedThroughProperty("Label226")]
	private Label _Label226;

	[AccessedThroughProperty("Label227")]
	private Label _Label227;

	[AccessedThroughProperty("Label228")]
	private Label _Label228;

	[AccessedThroughProperty("MicFreqEQ4")]
	private DomainUpDown _MicFreqEQ4;

	[AccessedThroughProperty("MicGainEQ4")]
	private DomainUpDown _MicGainEQ4;

	[AccessedThroughProperty("MicQEQ4")]
	private DomainUpDown _MicQEQ4;

	[AccessedThroughProperty("MicTrackBar4")]
	private TrackBar _MicTrackBar4;

	[AccessedThroughProperty("SuperTabControlPanel")]
	private SuperTabControlPanel _SuperTabControlPanel;

	[AccessedThroughProperty("输入")]
	private SuperTabItem _输入;

	[AccessedThroughProperty("Panel22")]
	private Panel _Panel22;

	[AccessedThroughProperty("MicHS20")]
	private ButtonX _MicHS20;

	[AccessedThroughProperty("MicLS20")]
	private ButtonX _MicLS20;

	[AccessedThroughProperty("MicP20")]
	private ButtonX _MicP20;

	[AccessedThroughProperty("Label229")]
	private Label _Label229;

	[AccessedThroughProperty("Label230")]
	private Label _Label230;

	[AccessedThroughProperty("Label231")]
	private Label _Label231;

	[AccessedThroughProperty("MicFreqEQ20")]
	private DomainUpDown _MicFreqEQ20;

	[AccessedThroughProperty("MicGainEQ20")]
	private DomainUpDown _MicGainEQ20;

	[AccessedThroughProperty("MicQEQ20")]
	private DomainUpDown _MicQEQ20;

	[AccessedThroughProperty("MicTrackBar20")]
	private TrackBar _MicTrackBar20;

	[AccessedThroughProperty("Panel23")]
	private Panel _Panel23;

	[AccessedThroughProperty("MicHS15")]
	private ButtonX _MicHS15;

	[AccessedThroughProperty("MicLS15")]
	private ButtonX _MicLS15;

	[AccessedThroughProperty("MicP15")]
	private ButtonX _MicP15;

	[AccessedThroughProperty("Label232")]
	private Label _Label232;

	[AccessedThroughProperty("Label233")]
	private Label _Label233;

	[AccessedThroughProperty("Label234")]
	private Label _Label234;

	[AccessedThroughProperty("MicFreqEQ15")]
	private DomainUpDown _MicFreqEQ15;

	[AccessedThroughProperty("MicGainEQ15")]
	private DomainUpDown _MicGainEQ15;

	[AccessedThroughProperty("MicQEQ15")]
	private DomainUpDown _MicQEQ15;

	[AccessedThroughProperty("MicTrackBar15")]
	private TrackBar _MicTrackBar15;

	[AccessedThroughProperty("Panel24")]
	private Panel _Panel24;

	[AccessedThroughProperty("MicHS12")]
	private ButtonX _MicHS12;

	[AccessedThroughProperty("MicLS12")]
	private ButtonX _MicLS12;

	[AccessedThroughProperty("MicP12")]
	private ButtonX _MicP12;

	[AccessedThroughProperty("Label235")]
	private Label _Label235;

	[AccessedThroughProperty("Label236")]
	private Label _Label236;

	[AccessedThroughProperty("Label237")]
	private Label _Label237;

	[AccessedThroughProperty("MicFreqEQ12")]
	private DomainUpDown _MicFreqEQ12;

	[AccessedThroughProperty("MicGainEQ12")]
	private DomainUpDown _MicGainEQ12;

	[AccessedThroughProperty("MicQEQ12")]
	private DomainUpDown _MicQEQ12;

	[AccessedThroughProperty("MicTrackBar12")]
	private TrackBar _MicTrackBar12;

	[AccessedThroughProperty("Panel25")]
	private Panel _Panel25;

	[AccessedThroughProperty("MicHS18")]
	private ButtonX _MicHS18;

	[AccessedThroughProperty("MicLS18")]
	private ButtonX _MicLS18;

	[AccessedThroughProperty("MicP18")]
	private ButtonX _MicP18;

	[AccessedThroughProperty("Label238")]
	private Label _Label238;

	[AccessedThroughProperty("Label239")]
	private Label _Label239;

	[AccessedThroughProperty("Label240")]
	private Label _Label240;

	[AccessedThroughProperty("MicFreqEQ18")]
	private DomainUpDown _MicFreqEQ18;

	[AccessedThroughProperty("MicGainEQ18")]
	private DomainUpDown _MicGainEQ18;

	[AccessedThroughProperty("MicQEQ18")]
	private DomainUpDown _MicQEQ18;

	[AccessedThroughProperty("MicTrackBar18")]
	private TrackBar _MicTrackBar18;

	[AccessedThroughProperty("Panel26")]
	private Panel _Panel26;

	[AccessedThroughProperty("MicHS16")]
	private ButtonX _MicHS16;

	[AccessedThroughProperty("MicLS16")]
	private ButtonX _MicLS16;

	[AccessedThroughProperty("MicP16")]
	private ButtonX _MicP16;

	[AccessedThroughProperty("Label241")]
	private Label _Label241;

	[AccessedThroughProperty("Label242")]
	private Label _Label242;

	[AccessedThroughProperty("Label243")]
	private Label _Label243;

	[AccessedThroughProperty("MicFreqEQ16")]
	private DomainUpDown _MicFreqEQ16;

	[AccessedThroughProperty("MicGainEQ16")]
	private DomainUpDown _MicGainEQ16;

	[AccessedThroughProperty("MicTrackBar16")]
	private TrackBar _MicTrackBar16;

	[AccessedThroughProperty("MicHS11")]
	private ButtonX _MicHS11;

	[AccessedThroughProperty("MicLS11")]
	private ButtonX _MicLS11;

	[AccessedThroughProperty("MicP11")]
	private ButtonX _MicP11;

	[AccessedThroughProperty("Label244")]
	private Label _Label244;

	[AccessedThroughProperty("Label245")]
	private Label _Label245;

	[AccessedThroughProperty("Label246")]
	private Label _Label246;

	[AccessedThroughProperty("MicFreqEQ11")]
	private DomainUpDown _MicFreqEQ11;

	[AccessedThroughProperty("MicQEQ11")]
	private DomainUpDown _MicQEQ11;

	[AccessedThroughProperty("MicTrackBar11")]
	private TrackBar _MicTrackBar11;

	[AccessedThroughProperty("Panel28")]
	private Panel _Panel28;

	[AccessedThroughProperty("MicHS13")]
	private ButtonX _MicHS13;

	[AccessedThroughProperty("MicLS13")]
	private ButtonX _MicLS13;

	[AccessedThroughProperty("MicP13")]
	private ButtonX _MicP13;

	[AccessedThroughProperty("Label247")]
	private Label _Label247;

	[AccessedThroughProperty("Label248")]
	private Label _Label248;

	[AccessedThroughProperty("Label249")]
	private Label _Label249;

	[AccessedThroughProperty("MicFreqEQ13")]
	private DomainUpDown _MicFreqEQ13;

	[AccessedThroughProperty("MicGainEQ13")]
	private DomainUpDown _MicGainEQ13;

	[AccessedThroughProperty("MicQEQ13")]
	private DomainUpDown _MicQEQ13;

	[AccessedThroughProperty("MicTrackBar13")]
	private TrackBar _MicTrackBar13;

	[AccessedThroughProperty("Panel29")]
	private Panel _Panel29;

	[AccessedThroughProperty("MicHS17")]
	private ButtonX _MicHS17;

	[AccessedThroughProperty("MicLS17")]
	private ButtonX _MicLS17;

	[AccessedThroughProperty("MicP17")]
	private ButtonX _MicP17;

	[AccessedThroughProperty("Label250")]
	private Label _Label250;

	[AccessedThroughProperty("Label251")]
	private Label _Label251;

	[AccessedThroughProperty("Label252")]
	private Label _Label252;

	[AccessedThroughProperty("MicFreqEQ17")]
	private DomainUpDown _MicFreqEQ17;

	[AccessedThroughProperty("MicGainEQ17")]
	private DomainUpDown _MicGainEQ17;

	[AccessedThroughProperty("Panel30")]
	private Panel _Panel30;

	[AccessedThroughProperty("MicHS19")]
	private ButtonX _MicHS19;

	[AccessedThroughProperty("MicLS19")]
	private ButtonX _MicLS19;

	[AccessedThroughProperty("MicP19")]
	private ButtonX _MicP19;

	[AccessedThroughProperty("Label253")]
	private Label _Label253;

	[AccessedThroughProperty("Label254")]
	private Label _Label254;

	[AccessedThroughProperty("Label255")]
	private Label _Label255;

	[AccessedThroughProperty("MicFreqEQ19")]
	private DomainUpDown _MicFreqEQ19;

	[AccessedThroughProperty("MicGainEQ19")]
	private DomainUpDown _MicGainEQ19;

	[AccessedThroughProperty("MicTrackBar19")]
	private TrackBar _MicTrackBar19;

	[AccessedThroughProperty("Panel31")]
	private Panel _Panel31;

	[AccessedThroughProperty("MicHS14")]
	private ButtonX _MicHS14;

	[AccessedThroughProperty("MicLS14")]
	private ButtonX _MicLS14;

	[AccessedThroughProperty("MicP14")]
	private ButtonX _MicP14;

	[AccessedThroughProperty("Label256")]
	private Label _Label256;

	[AccessedThroughProperty("Label257")]
	private Label _Label257;

	[AccessedThroughProperty("Label258")]
	private Label _Label258;

	[AccessedThroughProperty("MicFreqEQ14")]
	private DomainUpDown _MicFreqEQ14;

	[AccessedThroughProperty("MicGainEQ14")]
	private DomainUpDown _MicGainEQ14;

	[AccessedThroughProperty("MicQEQ14")]
	private DomainUpDown _MicQEQ14;

	[AccessedThroughProperty("MicTrackBar14")]
	private TrackBar _MicTrackBar14;

	[AccessedThroughProperty("Mic2HS20")]
	private ButtonX _Mic2HS20;

	[AccessedThroughProperty("Mic2LS20")]
	private ButtonX _Mic2LS20;

	[AccessedThroughProperty("Mic2P20")]
	private ButtonX _Mic2P20;

	[AccessedThroughProperty("Label285")]
	private Label _Label285;

	[AccessedThroughProperty("Label286")]
	private Label _Label286;

	[AccessedThroughProperty("Label287")]
	private Label _Label287;

	[AccessedThroughProperty("Mic2FreqEQ20")]
	private DomainUpDown _Mic2FreqEQ20;

	[AccessedThroughProperty("Mic2GainEQ20")]
	private DomainUpDown _Mic2GainEQ20;

	[AccessedThroughProperty("Mic2QEQ20")]
	private DomainUpDown _Mic2QEQ20;

	[AccessedThroughProperty("Mic2TrackBar20")]
	private TrackBar _Mic2TrackBar20;

	[AccessedThroughProperty("Panel33")]
	private Panel _Panel33;

	[AccessedThroughProperty("Mic2HS15")]
	private ButtonX _Mic2HS15;

	[AccessedThroughProperty("Mic2LS15")]
	private ButtonX _Mic2LS15;

	[AccessedThroughProperty("Mic2P15")]
	private ButtonX _Mic2P15;

	[AccessedThroughProperty("Label288")]
	private Label _Label288;

	[AccessedThroughProperty("Label289")]
	private Label _Label289;

	[AccessedThroughProperty("Label290")]
	private Label _Label290;

	[AccessedThroughProperty("Mic2FreqEQ15")]
	private DomainUpDown _Mic2FreqEQ15;

	[AccessedThroughProperty("Mic2GainEQ15")]
	private DomainUpDown _Mic2GainEQ15;

	[AccessedThroughProperty("Mic2QEQ15")]
	private DomainUpDown _Mic2QEQ15;

	[AccessedThroughProperty("Mic2HS12")]
	private ButtonX _Mic2HS12;

	[AccessedThroughProperty("Mic2LS12")]
	private ButtonX _Mic2LS12;

	[AccessedThroughProperty("Mic2P12")]
	private ButtonX _Mic2P12;

	[AccessedThroughProperty("Label291")]
	private Label _Label291;

	[AccessedThroughProperty("Label292")]
	private Label _Label292;

	[AccessedThroughProperty("Label293")]
	private Label _Label293;

	[AccessedThroughProperty("Mic2FreqEQ12")]
	private DomainUpDown _Mic2FreqEQ12;

	[AccessedThroughProperty("Mic2GainEQ12")]
	private DomainUpDown _Mic2GainEQ12;

	[AccessedThroughProperty("Mic2QEQ12")]
	private DomainUpDown _Mic2QEQ12;

	[AccessedThroughProperty("Panel35")]
	private Panel _Panel35;

	[AccessedThroughProperty("Mic2HS18")]
	private ButtonX _Mic2HS18;

	[AccessedThroughProperty("Mic2LS18")]
	private ButtonX _Mic2LS18;

	[AccessedThroughProperty("Mic2P18")]
	private ButtonX _Mic2P18;

	[AccessedThroughProperty("Label294")]
	private Label _Label294;

	[AccessedThroughProperty("Label295")]
	private Label _Label295;

	[AccessedThroughProperty("Label296")]
	private Label _Label296;

	[AccessedThroughProperty("Mic2FreqEQ18")]
	private DomainUpDown _Mic2FreqEQ18;

	[AccessedThroughProperty("Mic2GainEQ18")]
	private DomainUpDown _Mic2GainEQ18;

	[AccessedThroughProperty("Mic2QEQ18")]
	private DomainUpDown _Mic2QEQ18;

	[AccessedThroughProperty("Mic2TrackBar18")]
	private TrackBar _Mic2TrackBar18;

	[AccessedThroughProperty("Panel36")]
	private Panel _Panel36;

	[AccessedThroughProperty("Mic2HS16")]
	private ButtonX _Mic2HS16;

	[AccessedThroughProperty("Mic2LS16")]
	private ButtonX _Mic2LS16;

	[AccessedThroughProperty("Mic2P16")]
	private ButtonX _Mic2P16;

	[AccessedThroughProperty("Label297")]
	private Label _Label297;

	[AccessedThroughProperty("Label298")]
	private Label _Label298;

	[AccessedThroughProperty("Label299")]
	private Label _Label299;

	[AccessedThroughProperty("Mic2FreqEQ16")]
	private DomainUpDown _Mic2FreqEQ16;

	[AccessedThroughProperty("Mic2GainEQ16")]
	private DomainUpDown _Mic2GainEQ16;

	[AccessedThroughProperty("Mic2QEQ16")]
	private DomainUpDown _Mic2QEQ16;

	[AccessedThroughProperty("Mic2TrackBar16")]
	private TrackBar _Mic2TrackBar16;

	[AccessedThroughProperty("Mic2HS11")]
	private ButtonX _Mic2HS11;

	[AccessedThroughProperty("Mic2LS11")]
	private ButtonX _Mic2LS11;

	[AccessedThroughProperty("Mic2P11")]
	private ButtonX _Mic2P11;

	[AccessedThroughProperty("Label300")]
	private Label _Label300;

	[AccessedThroughProperty("Label301")]
	private Label _Label301;

	[AccessedThroughProperty("Label302")]
	private Label _Label302;

	[AccessedThroughProperty("Mic2FreqEQ11")]
	private DomainUpDown _Mic2FreqEQ11;

	[AccessedThroughProperty("Mic2GainEQ11")]
	private DomainUpDown _Mic2GainEQ11;

	[AccessedThroughProperty("Mic2QEQ11")]
	private DomainUpDown _Mic2QEQ11;

	[AccessedThroughProperty("Panel38")]
	private Panel _Panel38;

	[AccessedThroughProperty("Mic2HS13")]
	private ButtonX _Mic2HS13;

	[AccessedThroughProperty("Mic2LS13")]
	private ButtonX _Mic2LS13;

	[AccessedThroughProperty("Mic2P13")]
	private ButtonX _Mic2P13;

	[AccessedThroughProperty("Label303")]
	private Label _Label303;

	[AccessedThroughProperty("Label304")]
	private Label _Label304;

	[AccessedThroughProperty("Label305")]
	private Label _Label305;

	[AccessedThroughProperty("Mic2FreqEQ13")]
	private DomainUpDown _Mic2FreqEQ13;

	[AccessedThroughProperty("Mic2GainEQ13")]
	private DomainUpDown _Mic2GainEQ13;

	[AccessedThroughProperty("Mic2QEQ13")]
	private DomainUpDown _Mic2QEQ13;

	[AccessedThroughProperty("Mic2TrackBar13")]
	private TrackBar _Mic2TrackBar13;

	[AccessedThroughProperty("Panel39")]
	private Panel _Panel39;

	[AccessedThroughProperty("Mic2HS17")]
	private ButtonX _Mic2HS17;

	[AccessedThroughProperty("Mic2LS17")]
	private ButtonX _Mic2LS17;

	[AccessedThroughProperty("Mic2P17")]
	private ButtonX _Mic2P17;

	[AccessedThroughProperty("Label306")]
	private Label _Label306;

	[AccessedThroughProperty("Label307")]
	private Label _Label307;

	[AccessedThroughProperty("Label308")]
	private Label _Label308;

	[AccessedThroughProperty("Mic2FreqEQ17")]
	private DomainUpDown _Mic2FreqEQ17;

	[AccessedThroughProperty("Mic2GainEQ17")]
	private DomainUpDown _Mic2GainEQ17;

	[AccessedThroughProperty("Mic2QEQ17")]
	private DomainUpDown _Mic2QEQ17;

	[AccessedThroughProperty("Mic2TrackBar17")]
	private TrackBar _Mic2TrackBar17;

	[AccessedThroughProperty("Panel40")]
	private Panel _Panel40;

	[AccessedThroughProperty("Mic2HS19")]
	private ButtonX _Mic2HS19;

	[AccessedThroughProperty("Mic2LS19")]
	private ButtonX _Mic2LS19;

	[AccessedThroughProperty("Mic2P19")]
	private ButtonX _Mic2P19;

	[AccessedThroughProperty("Label309")]
	private Label _Label309;

	[AccessedThroughProperty("Label310")]
	private Label _Label310;

	[AccessedThroughProperty("Label311")]
	private Label _Label311;

	[AccessedThroughProperty("Mic2FreqEQ19")]
	private DomainUpDown _Mic2FreqEQ19;

	[AccessedThroughProperty("Mic2GainEQ19")]
	private DomainUpDown _Mic2GainEQ19;

	[AccessedThroughProperty("Mic2QEQ19")]
	private DomainUpDown _Mic2QEQ19;

	[AccessedThroughProperty("Mic2TrackBar19")]
	private TrackBar _Mic2TrackBar19;

	[AccessedThroughProperty("Panel41")]
	private Panel _Panel41;

	[AccessedThroughProperty("Mic2HS14")]
	private ButtonX _Mic2HS14;

	[AccessedThroughProperty("Mic2LS14")]
	private ButtonX _Mic2LS14;

	[AccessedThroughProperty("Mic2P14")]
	private ButtonX _Mic2P14;

	[AccessedThroughProperty("Label312")]
	private Label _Label312;

	[AccessedThroughProperty("Label313")]
	private Label _Label313;

	[AccessedThroughProperty("Label314")]
	private Label _Label314;

	[AccessedThroughProperty("Mic2FreqEQ14")]
	private DomainUpDown _Mic2FreqEQ14;

	[AccessedThroughProperty("Mic2GainEQ14")]
	private DomainUpDown _Mic2GainEQ14;

	[AccessedThroughProperty("Mic2QEQ14")]
	private DomainUpDown _Mic2QEQ14;

	[AccessedThroughProperty("MicGainEQ11")]
	private DomainUpDown _MicGainEQ11;

	[AccessedThroughProperty("EchoHighDampLable")]
	private Label _EchoHighDampLable;

	[AccessedThroughProperty("Label5")]
	private Label _Label5;

	[AccessedThroughProperty("EchoHighDamp")]
	private TrackBar _EchoHighDamp;

	[AccessedThroughProperty("SuperTabControlPanel11")]
	private SuperTabControlPanel _SuperTabControlPanel11;

	[AccessedThroughProperty("GroupBox2")]
	private GroupBox _GroupBox2;

	[AccessedThroughProperty("RecRevVolPhase")]
	private Label _RecRevVolPhase;

	[AccessedThroughProperty("RecRevVol")]
	private TrackBar _RecRevVol;

	[AccessedThroughProperty("RecRevVolLable")]
	private Label _RecRevVolLable;

	[AccessedThroughProperty("RecEchoVolPhase")]
	private Label _RecEchoVolPhase;

	[AccessedThroughProperty("RecEchoVolLable")]
	private Label _RecEchoVolLable;

	[AccessedThroughProperty("RecMicVolPhase")]
	private Label _RecMicVolPhase;

	[AccessedThroughProperty("RecMicVolLable")]
	private Label _RecMicVolLable;

	[AccessedThroughProperty("RecMusicVolPhase")]
	private Label _RecMusicVolPhase;

	[AccessedThroughProperty("RecMusicVolLable")]
	private Label _RecMusicVolLable;

	[AccessedThroughProperty("Label18")]
	private Label _Label18;

	[AccessedThroughProperty("Label19")]
	private Label _Label19;

	[AccessedThroughProperty("Label20")]
	private Label _Label20;

	[AccessedThroughProperty("Label21")]
	private Label _Label21;

	[AccessedThroughProperty("RecEchoVol")]
	private TrackBar _RecEchoVol;

	[AccessedThroughProperty("RecMicVol")]
	private TrackBar _RecMicVol;

	[AccessedThroughProperty("RecMusicVol")]
	private TrackBar _RecMusicVol;

	[AccessedThroughProperty("MicTrackBar17")]
	private TrackBar _MicTrackBar17;

	[AccessedThroughProperty("Mic2TrackBar15")]
	private TrackBar _Mic2TrackBar15;

	[AccessedThroughProperty("Mic2TrackBar12")]
	private TrackBar _Mic2TrackBar12;

	[AccessedThroughProperty("Mic2TrackBar11")]
	private TrackBar _Mic2TrackBar11;

	[AccessedThroughProperty("Mic2TrackBar14")]
	private TrackBar _Mic2TrackBar14;

	[AccessedThroughProperty("Panel1")]
	private Panel _Panel1;

	[AccessedThroughProperty("Panel86")]
	private Panel _Panel86;

	[AccessedThroughProperty("Panel85")]
	private Panel _Panel85;

	[AccessedThroughProperty("Panel87")]
	private Panel _Panel87;

	[AccessedThroughProperty("Panel88")]
	private Panel _Panel88;

	[AccessedThroughProperty("Panel91")]
	private Panel _Panel91;

	[AccessedThroughProperty("Panel90")]
	private Panel _Panel90;

	[AccessedThroughProperty("MainMuteR")]
	private Button _MainMuteR;

	[AccessedThroughProperty("MainMuteL")]
	private Button _MainMuteL;

	[AccessedThroughProperty("SubMute")]
	private Button _SubMute;

	[AccessedThroughProperty("CenMute")]
	private Button _CenMute;

	[AccessedThroughProperty("EQBypassRev")]
	private ButtonX _EQBypassRev;

	[AccessedThroughProperty("EQResetRev")]
	private ButtonX _EQResetRev;

	[AccessedThroughProperty("EQBypassMicA")]
	private ButtonX _EQBypassMicA;

	[AccessedThroughProperty("EQResetMicA")]
	private ButtonX _EQResetMicA;

	[AccessedThroughProperty("EQBypassEcho")]
	private ButtonX _EQBypassEcho;

	[AccessedThroughProperty("EQResetEcho")]
	private ButtonX _EQResetEcho;

	[AccessedThroughProperty("EQBypassMicB")]
	private ButtonX _EQBypassMicB;

	[AccessedThroughProperty("EQResetMicB")]
	private ButtonX _EQResetMicB;

	[AccessedThroughProperty("EQBypassSub")]
	private ButtonX _EQBypassSub;

	[AccessedThroughProperty("EQResetSub")]
	private ButtonX _EQResetSub;

	[AccessedThroughProperty("EQBypassCen")]
	private ButtonX _EQBypassCen;

	[AccessedThroughProperty("EQResetCen")]
	private ButtonX _EQResetCen;

	[AccessedThroughProperty("EQBypassMain")]
	private ButtonX _EQBypassMain;

	[AccessedThroughProperty("EQResetMain")]
	private ButtonX _EQResetMain;

	[AccessedThroughProperty("SuperTabControlPanel12")]
	private SuperTabControlPanel _SuperTabControlPanel12;

	[AccessedThroughProperty("SuperTabControlPanel13")]
	private SuperTabControlPanel _SuperTabControlPanel13;

	[AccessedThroughProperty("Panel98")]
	private Panel _Panel98;

	[AccessedThroughProperty("Label67")]
	private Label _Label67;

	[AccessedThroughProperty("Label87")]
	private Label _Label87;

	[AccessedThroughProperty("Label88")]
	private Label _Label88;

	[AccessedThroughProperty("Label89")]
	private Label _Label89;

	[AccessedThroughProperty("Label93")]
	private Label _Label93;

	[AccessedThroughProperty("Panel92")]
	private Panel _Panel92;

	[AccessedThroughProperty("Label8")]
	private Label _Label8;

	[AccessedThroughProperty("Label41")]
	private Label _Label41;

	[AccessedThroughProperty("Label42")]
	private Label _Label42;

	[AccessedThroughProperty("Label43")]
	private Label _Label43;

	[AccessedThroughProperty("Label47")]
	private Label _Label47;

	[AccessedThroughProperty("EQBypassRevB")]
	private ButtonX _EQBypassRevB;

	[AccessedThroughProperty("EQResetRevB")]
	private ButtonX _EQResetRevB;

	[AccessedThroughProperty("RevBTimeLable")]
	private Label _RevBTimeLable;

	[AccessedThroughProperty("RevBPreDelayLable")]
	private Label _RevBPreDelayLable;

	[AccessedThroughProperty("Label70")]
	private Label _Label70;

	[AccessedThroughProperty("Label71")]
	private Label _Label71;

	[AccessedThroughProperty("RevBTime")]
	private TrackBar _RevBTime;

	[AccessedThroughProperty("RevBPreDelay")]
	private TrackBar _RevBPreDelay;

	[AccessedThroughProperty("RevBLpfLable")]
	private Label _RevBLpfLable;

	[AccessedThroughProperty("RevBHpfLable")]
	private Label _RevBHpfLable;

	[AccessedThroughProperty("Label74")]
	private Label _Label74;

	[AccessedThroughProperty("RevBLpf")]
	private TrackBar _RevBLpf;

	[AccessedThroughProperty("Label77")]
	private Label _Label77;

	[AccessedThroughProperty("RevBHpf")]
	private TrackBar _RevBHpf;

	[AccessedThroughProperty("RevBDirectVolPhase")]
	private Label _RevBDirectVolPhase;

	[AccessedThroughProperty("RevBDirectVolLable")]
	private Label _RevBDirectVolLable;

	[AccessedThroughProperty("RevBVolPhase")]
	private Label _RevBVolPhase;

	[AccessedThroughProperty("RevBVolLable")]
	private Label _RevBVolLable;

	[AccessedThroughProperty("Label85")]
	private Label _Label85;

	[AccessedThroughProperty("Label86")]
	private Label _Label86;

	[AccessedThroughProperty("RevBDirectVolTrabar")]
	private TrackBar _RevBDirectVolTrabar;

	[AccessedThroughProperty("RevBVolTrabar")]
	private TrackBar _RevBVolTrabar;

	[AccessedThroughProperty("Panel99")]
	private Panel _Panel99;

	[AccessedThroughProperty("RevBHS5")]
	private ButtonX _RevBHS5;

	[AccessedThroughProperty("RevBLS5")]
	private ButtonX _RevBLS5;

	[AccessedThroughProperty("RevBP5")]
	private ButtonX _RevBP5;

	[AccessedThroughProperty("Label90")]
	private Label _Label90;

	[AccessedThroughProperty("Label91")]
	private Label _Label91;

	[AccessedThroughProperty("Label92")]
	private Label _Label92;

	[AccessedThroughProperty("RevBFreqEQ5")]
	private DomainUpDown _RevBFreqEQ5;

	[AccessedThroughProperty("RevBGainEQ5")]
	private DomainUpDown _RevBGainEQ5;

	[AccessedThroughProperty("RevBQEQ5")]
	private DomainUpDown _RevBQEQ5;

	[AccessedThroughProperty("RevBTrackBar5")]
	private TrackBar _RevBTrackBar5;

	[AccessedThroughProperty("Panel100")]
	private Panel _Panel100;

	[AccessedThroughProperty("RevBHS2")]
	private ButtonX _RevBHS2;

	[AccessedThroughProperty("RevBLS2")]
	private ButtonX _RevBLS2;

	[AccessedThroughProperty("RevBP2")]
	private ButtonX _RevBP2;

	[AccessedThroughProperty("Label94")]
	private Label _Label94;

	[AccessedThroughProperty("Label95")]
	private Label _Label95;

	[AccessedThroughProperty("Label96")]
	private Label _Label96;

	[AccessedThroughProperty("RevBFreqEQ2")]
	private DomainUpDown _RevBFreqEQ2;

	[AccessedThroughProperty("RevBGainEQ2")]
	private DomainUpDown _RevBGainEQ2;

	[AccessedThroughProperty("RevBQEQ2")]
	private DomainUpDown _RevBQEQ2;

	[AccessedThroughProperty("RevBTrackBar2")]
	private TrackBar _RevBTrackBar2;

	[AccessedThroughProperty("Panel101")]
	private Panel _Panel101;

	[AccessedThroughProperty("RevBHS1")]
	private ButtonX _RevBHS1;

	[AccessedThroughProperty("RevBLS1")]
	private ButtonX _RevBLS1;

	[AccessedThroughProperty("RevBP1")]
	private ButtonX _RevBP1;

	[AccessedThroughProperty("Label97")]
	private Label _Label97;

	[AccessedThroughProperty("Label98")]
	private Label _Label98;

	[AccessedThroughProperty("Label99")]
	private Label _Label99;

	[AccessedThroughProperty("RevBFreqEQ1")]
	private DomainUpDown _RevBFreqEQ1;

	[AccessedThroughProperty("RevBGainEQ1")]
	private DomainUpDown _RevBGainEQ1;

	[AccessedThroughProperty("RevBQEQ1")]
	private DomainUpDown _RevBQEQ1;

	[AccessedThroughProperty("RevBTrackBar1")]
	private TrackBar _RevBTrackBar1;

	[AccessedThroughProperty("Panel102")]
	private Panel _Panel102;

	[AccessedThroughProperty("RevBHS3")]
	private ButtonX _RevBHS3;

	[AccessedThroughProperty("RevBLS3")]
	private ButtonX _RevBLS3;

	[AccessedThroughProperty("RevBP3")]
	private ButtonX _RevBP3;

	[AccessedThroughProperty("Label100")]
	private Label _Label100;

	[AccessedThroughProperty("Label101")]
	private Label _Label101;

	[AccessedThroughProperty("Label102")]
	private Label _Label102;

	[AccessedThroughProperty("RevBFreqEQ3")]
	private DomainUpDown _RevBFreqEQ3;

	[AccessedThroughProperty("RevBGainEQ3")]
	private DomainUpDown _RevBGainEQ3;

	[AccessedThroughProperty("RevBQEQ3")]
	private DomainUpDown _RevBQEQ3;

	[AccessedThroughProperty("RevBTrackBar3")]
	private TrackBar _RevBTrackBar3;

	[AccessedThroughProperty("Panel103")]
	private Panel _Panel103;

	[AccessedThroughProperty("RevBHS4")]
	private ButtonX _RevBHS4;

	[AccessedThroughProperty("RevBLS4")]
	private ButtonX _RevBLS4;

	[AccessedThroughProperty("RevBP4")]
	private ButtonX _RevBP4;

	[AccessedThroughProperty("Label103")]
	private Label _Label103;

	[AccessedThroughProperty("Label104")]
	private Label _Label104;

	[AccessedThroughProperty("Label105")]
	private Label _Label105;

	[AccessedThroughProperty("RevBFreqEQ4")]
	private DomainUpDown _RevBFreqEQ4;

	[AccessedThroughProperty("RevBGainEQ4")]
	private DomainUpDown _RevBGainEQ4;

	[AccessedThroughProperty("RevBQEQ4")]
	private DomainUpDown _RevBQEQ4;

	[AccessedThroughProperty("RevBTrackBar4")]
	private TrackBar _RevBTrackBar4;

	[AccessedThroughProperty("EQBypassEchoB")]
	private ButtonX _EQBypassEchoB;

	[AccessedThroughProperty("EQResetEchoB")]
	private ButtonX _EQResetEchoB;

	[AccessedThroughProperty("EchoBDelayRLable")]
	private Label _EchoBDelayRLable;

	[AccessedThroughProperty("EchoBPreDelayRLable")]
	private Label _EchoBPreDelayRLable;

	[AccessedThroughProperty("EchoBRepeatLable")]
	private Label _EchoBRepeatLable;

	[AccessedThroughProperty("EchoBDelayLable")]
	private Label _EchoBDelayLable;

	[AccessedThroughProperty("EchoBPreDelayLable")]
	private Label _EchoBPreDelayLable;

	[AccessedThroughProperty("Label16")]
	private Label _Label16;

	[AccessedThroughProperty("Label17")]
	private Label _Label17;

	[AccessedThroughProperty("Label26")]
	private Label _Label26;

	[AccessedThroughProperty("Label27")]
	private Label _Label27;

	[AccessedThroughProperty("Label28")]
	private Label _Label28;

	[AccessedThroughProperty("EchoBDelayR")]
	private TrackBar _EchoBDelayR;

	[AccessedThroughProperty("EchoBPreDelayR")]
	private TrackBar _EchoBPreDelayR;

	[AccessedThroughProperty("EchoBRepeat")]
	private TrackBar _EchoBRepeat;

	[AccessedThroughProperty("EchoBDelay")]
	private TrackBar _EchoBDelay;

	[AccessedThroughProperty("EchoBPreDelay")]
	private TrackBar _EchoBPreDelay;

	[AccessedThroughProperty("EchoBHighDampLable")]
	private Label _EchoBHighDampLable;

	[AccessedThroughProperty("EchoBLpfLable")]
	private Label _EchoBLpfLable;

	[AccessedThroughProperty("EchoBHpfLable")]
	private Label _EchoBHpfLable;

	[AccessedThroughProperty("Label32")]
	private Label _Label32;

	[AccessedThroughProperty("Label33")]
	private Label _Label33;

	[AccessedThroughProperty("EchoBHighDamp")]
	private TrackBar _EchoBHighDamp;

	[AccessedThroughProperty("EchoBLpf")]
	private TrackBar _EchoBLpf;

	[AccessedThroughProperty("Label34")]
	private Label _Label34;

	[AccessedThroughProperty("EchoBHpf")]
	private TrackBar _EchoBHpf;

	[AccessedThroughProperty("EchoBDirectVolPhase")]
	private Label _EchoBDirectVolPhase;

	[AccessedThroughProperty("EchoBDirectVolLable")]
	private Label _EchoBDirectVolLable;

	[AccessedThroughProperty("EchoBVolPhase")]
	private Label _EchoBVolPhase;

	[AccessedThroughProperty("EchoBVolLable")]
	private Label _EchoBVolLable;

	[AccessedThroughProperty("Label39")]
	private Label _Label39;

	[AccessedThroughProperty("Label40")]
	private Label _Label40;

	[AccessedThroughProperty("EchoBDirectVolTrabar")]
	private TrackBar _EchoBDirectVolTrabar;

	[AccessedThroughProperty("EchoBVolTrabar")]
	private TrackBar _EchoBVolTrabar;

	[AccessedThroughProperty("Panel93")]
	private Panel _Panel93;

	[AccessedThroughProperty("EchoBHS5")]
	private ButtonX _EchoBHS5;

	[AccessedThroughProperty("EchoBLS5")]
	private ButtonX _EchoBLS5;

	[AccessedThroughProperty("EchoBP5")]
	private ButtonX _EchoBP5;

	[AccessedThroughProperty("Label44")]
	private Label _Label44;

	[AccessedThroughProperty("Label45")]
	private Label _Label45;

	[AccessedThroughProperty("Label46")]
	private Label _Label46;

	[AccessedThroughProperty("EchoBFreqEQ5")]
	private DomainUpDown _EchoBFreqEQ5;

	[AccessedThroughProperty("EchoBGainEQ5")]
	private DomainUpDown _EchoBGainEQ5;

	[AccessedThroughProperty("EchoBQEQ5")]
	private DomainUpDown _EchoBQEQ5;

	[AccessedThroughProperty("EchoBTrackBar5")]
	private TrackBar _EchoBTrackBar5;

	[AccessedThroughProperty("Panel94")]
	private Panel _Panel94;

	[AccessedThroughProperty("EchoBHS2")]
	private ButtonX _EchoBHS2;

	[AccessedThroughProperty("EchoBLS2")]
	private ButtonX _EchoBLS2;

	[AccessedThroughProperty("EchoBP2")]
	private ButtonX _EchoBP2;

	[AccessedThroughProperty("Label52")]
	private Label _Label52;

	[AccessedThroughProperty("Label53")]
	private Label _Label53;

	[AccessedThroughProperty("Label56")]
	private Label _Label56;

	[AccessedThroughProperty("EchoBFreqEQ2")]
	private DomainUpDown _EchoBFreqEQ2;

	[AccessedThroughProperty("EchoBGainEQ2")]
	private DomainUpDown _EchoBGainEQ2;

	[AccessedThroughProperty("EchoBQEQ2")]
	private DomainUpDown _EchoBQEQ2;

	[AccessedThroughProperty("EchoBTrackBar2")]
	private TrackBar _EchoBTrackBar2;

	[AccessedThroughProperty("Panel95")]
	private Panel _Panel95;

	[AccessedThroughProperty("EchoBHS1")]
	private ButtonX _EchoBHS1;

	[AccessedThroughProperty("EchoBLS1")]
	private ButtonX _EchoBLS1;

	[AccessedThroughProperty("EchoBP1")]
	private ButtonX _EchoBP1;

	[AccessedThroughProperty("Label57")]
	private Label _Label57;

	[AccessedThroughProperty("Label58")]
	private Label _Label58;

	[AccessedThroughProperty("Label60")]
	private Label _Label60;

	[AccessedThroughProperty("EchoBFreqEQ1")]
	private DomainUpDown _EchoBFreqEQ1;

	[AccessedThroughProperty("EchoBGainEQ1")]
	private DomainUpDown _EchoBGainEQ1;

	[AccessedThroughProperty("EchoBQEQ1")]
	private DomainUpDown _EchoBQEQ1;

	[AccessedThroughProperty("EchoBTrackBar1")]
	private TrackBar _EchoBTrackBar1;

	[AccessedThroughProperty("Panel96")]
	private Panel _Panel96;

	[AccessedThroughProperty("EchoBHS3")]
	private ButtonX _EchoBHS3;

	[AccessedThroughProperty("EchoBLS3")]
	private ButtonX _EchoBLS3;

	[AccessedThroughProperty("EchoBP3")]
	private ButtonX _EchoBP3;

	[AccessedThroughProperty("Label61")]
	private Label _Label61;

	[AccessedThroughProperty("Label62")]
	private Label _Label62;

	[AccessedThroughProperty("Label63")]
	private Label _Label63;

	[AccessedThroughProperty("EchoBFreqEQ3")]
	private DomainUpDown _EchoBFreqEQ3;

	[AccessedThroughProperty("EchoBGainEQ3")]
	private DomainUpDown _EchoBGainEQ3;

	[AccessedThroughProperty("EchoBQEQ3")]
	private DomainUpDown _EchoBQEQ3;

	[AccessedThroughProperty("EchoBTrackBar3")]
	private TrackBar _EchoBTrackBar3;

	[AccessedThroughProperty("Panel97")]
	private Panel _Panel97;

	[AccessedThroughProperty("EchoBHS4")]
	private ButtonX _EchoBHS4;

	[AccessedThroughProperty("EchoBLS4")]
	private ButtonX _EchoBLS4;

	[AccessedThroughProperty("EchoBP4")]
	private ButtonX _EchoBP4;

	[AccessedThroughProperty("Label64")]
	private Label _Label64;

	[AccessedThroughProperty("Label65")]
	private Label _Label65;

	[AccessedThroughProperty("Label66")]
	private Label _Label66;

	[AccessedThroughProperty("EchoBFreqEQ4")]
	private DomainUpDown _EchoBFreqEQ4;

	[AccessedThroughProperty("EchoBGainEQ4")]
	private DomainUpDown _EchoBGainEQ4;

	[AccessedThroughProperty("EchoBQEQ4")]
	private DomainUpDown _EchoBQEQ4;

	[AccessedThroughProperty("EchoBTrackBar4")]
	private TrackBar _EchoBTrackBar4;

	[AccessedThroughProperty("RevBPic")]
	private PictureBox _RevBPic;

	[AccessedThroughProperty("EchoBPic")]
	private PictureBox _EchoBPic;

	[AccessedThroughProperty("SubSolo")]
	private Button _SubSolo;

	[AccessedThroughProperty("CenSolo")]
	private Button _CenSolo;

	[AccessedThroughProperty("MainSoloR")]
	private Button _MainSoloR;

	[AccessedThroughProperty("MainSoloL")]
	private Button _MainSoloL;

	[AccessedThroughProperty("Panel104")]
	private Panel _Panel104;

	[AccessedThroughProperty("Panel105")]
	private Panel _Panel105;

	[AccessedThroughProperty("Panel106")]
	private Panel _Panel106;

	[AccessedThroughProperty("Panel107")]
	private Panel _Panel107;

	[AccessedThroughProperty("Panel108")]
	private Panel _Panel108;

	[AccessedThroughProperty("Panel109")]
	private Panel _Panel109;

	[AccessedThroughProperty("CenMusicDelay")]
	private TrackBar _CenMusicDelay;

	[AccessedThroughProperty("Label125")]
	private Label _Label125;

	[AccessedThroughProperty("Label119")]
	private Label _Label119;

	[AccessedThroughProperty("Label112")]
	private Label _Label112;

	[AccessedThroughProperty("Label123")]
	private Label _Label123;

	[AccessedThroughProperty("Label121")]
	private Label _Label121;

	[AccessedThroughProperty("Label111")]
	private Label _Label111;

	[AccessedThroughProperty("Label113")]
	private Label _Label113;

	[AccessedThroughProperty("Label122")]
	private Label _Label122;

	[AccessedThroughProperty("Label124")]
	private Label _Label124;

	[AccessedThroughProperty("Label114")]
	private Label _Label114;

	[AccessedThroughProperty("Label109")]
	private Label _Label109;

	[AccessedThroughProperty("Label83")]
	private Label _Label83;

	[AccessedThroughProperty("Label73")]
	private Label _Label73;

	[AccessedThroughProperty("Label107")]
	private Label _Label107;

	[AccessedThroughProperty("Label84")]
	private Label _Label84;

	[AccessedThroughProperty("Label72")]
	private Label _Label72;

	[AccessedThroughProperty("Label78")]
	private Label _Label78;

	[AccessedThroughProperty("Label106")]
	private Label _Label106;

	[AccessedThroughProperty("Label108")]
	private Label _Label108;

	[AccessedThroughProperty("Label82")]
	private Label _Label82;

	[AccessedThroughProperty("Label127")]
	private Label _Label127;

	[AccessedThroughProperty("Label147")]
	private Label _Label147;

	[AccessedThroughProperty("Label146")]
	private Label _Label146;

	[AccessedThroughProperty("Label143")]
	private Label _Label143;

	[AccessedThroughProperty("Label144")]
	private Label _Label144;

	[AccessedThroughProperty("Label142")]
	private Label _Label142;

	[AccessedThroughProperty("Label148")]
	private Label _Label148;

	[AccessedThroughProperty("Label141")]
	private Label _Label141;

	[AccessedThroughProperty("Label145")]
	private Label _Label145;

	[AccessedThroughProperty("Label150")]
	private Label _Label150;

	[AccessedThroughProperty("Label135")]
	private Label _Label135;

	[AccessedThroughProperty("Label133")]
	private Label _Label133;

	[AccessedThroughProperty("Label132")]
	private Label _Label132;

	[AccessedThroughProperty("Label128")]
	private Label _Label128;

	[AccessedThroughProperty("Label129")]
	private Label _Label129;

	[AccessedThroughProperty("Label149")]
	private Label _Label149;

	[AccessedThroughProperty("Label134")]
	private Label _Label134;

	[AccessedThroughProperty("Label126")]
	private Label _Label126;

	[AccessedThroughProperty("Label131")]
	private Label _Label131;

	[AccessedThroughProperty("Label136")]
	private Label _Label136;

	[AccessedThroughProperty("Label195")]
	private Label _Label195;

	[AccessedThroughProperty("Label193")]
	private Label _Label193;

	[AccessedThroughProperty("Label196")]
	private Label _Label196;

	[AccessedThroughProperty("Label191")]
	private Label _Label191;

	[AccessedThroughProperty("Label188")]
	private Label _Label188;

	[AccessedThroughProperty("Label186")]
	private Label _Label186;

	[AccessedThroughProperty("Label189")]
	private Label _Label189;

	[AccessedThroughProperty("Label190")]
	private Label _Label190;

	[AccessedThroughProperty("Label181")]
	private Label _Label181;

	[AccessedThroughProperty("Label177")]
	private Label _Label177;

	[AccessedThroughProperty("Label176")]
	private Label _Label176;

	[AccessedThroughProperty("Label178")]
	private Label _Label178;

	[AccessedThroughProperty("Label179")]
	private Label _Label179;

	[AccessedThroughProperty("Label175")]
	private Label _Label175;

	[AccessedThroughProperty("Label172")]
	private Label _Label172;

	[AccessedThroughProperty("Label171")]
	private Label _Label171;

	[AccessedThroughProperty("Label173")]
	private Label _Label173;

	[AccessedThroughProperty("Label174")]
	private Label _Label174;

	[AccessedThroughProperty("Label170")]
	private Label _Label170;

	[AccessedThroughProperty("Label167")]
	private Label _Label167;

	[AccessedThroughProperty("Label166")]
	private Label _Label166;

	[AccessedThroughProperty("Label168")]
	private Label _Label168;

	[AccessedThroughProperty("Label169")]
	private Label _Label169;

	[AccessedThroughProperty("Label157")]
	private Label _Label157;

	[AccessedThroughProperty("Label152")]
	private Label _Label152;

	[AccessedThroughProperty("Label151")]
	private Label _Label151;

	[AccessedThroughProperty("Label153")]
	private Label _Label153;

	[AccessedThroughProperty("Label155")]
	private Label _Label155;

	[AccessedThroughProperty("Label165")]
	private Label _Label165;

	[AccessedThroughProperty("Label159")]
	private Label _Label159;

	[AccessedThroughProperty("Label158")]
	private Label _Label158;

	[AccessedThroughProperty("Label163")]
	private Label _Label163;

	[AccessedThroughProperty("Label164")]
	private Label _Label164;

	[AccessedThroughProperty("PaneEchoA")]
	private Panel _PaneEchoA;

	[AccessedThroughProperty("PaneRevB")]
	private Panel _PaneRevB;

	[AccessedThroughProperty("PaneEchoB")]
	private Panel _PaneEchoB;

	[AccessedThroughProperty("PaneRevA")]
	private Panel _PaneRevA;

	[AccessedThroughProperty("Label265")]
	private Label _Label265;

	[AccessedThroughProperty("Label266")]
	private Label _Label266;

	[AccessedThroughProperty("SuperTabControlPanel15")]
	private SuperTabControlPanel _SuperTabControlPanel15;

	[AccessedThroughProperty("SuperTabControlPanel19")]
	private SuperTabControlPanel _SuperTabControlPanel19;

	[AccessedThroughProperty("GroupBox12")]
	private GroupBox _GroupBox12;

	[AccessedThroughProperty("ComboBox6")]
	private ComboBox _ComboBox6;

	[AccessedThroughProperty("Label478")]
	private Label _Label478;

	[AccessedThroughProperty("Label479")]
	private Label _Label479;

	[AccessedThroughProperty("Label480")]
	private Label _Label480;

	[AccessedThroughProperty("TrackBar26")]
	private TrackBar _TrackBar26;

	[AccessedThroughProperty("GroupBox13")]
	private GroupBox _GroupBox13;

	[AccessedThroughProperty("TrackBar27")]
	private TrackBar _TrackBar27;

	[AccessedThroughProperty("GroupBox14")]
	private GroupBox _GroupBox14;

	[AccessedThroughProperty("Button1")]
	private Button _Button1;

	[AccessedThroughProperty("Button2")]
	private Button _Button2;

	[AccessedThroughProperty("GroupBox15")]
	private GroupBox _GroupBox15;

	[AccessedThroughProperty("Label485")]
	private Label _Label485;

	[AccessedThroughProperty("Label486")]
	private Label _Label486;

	[AccessedThroughProperty("Label491")]
	private Label _Label491;

	[AccessedThroughProperty("Label501")]
	private Label _Label501;

	[AccessedThroughProperty("TrackBar28")]
	private TrackBar _TrackBar28;

	[AccessedThroughProperty("Label508")]
	private Label _Label508;

	[AccessedThroughProperty("TrackBar29")]
	private TrackBar _TrackBar29;

	[AccessedThroughProperty("TrackBar30")]
	private TrackBar _TrackBar30;

	[AccessedThroughProperty("Label509")]
	private Label _Label509;

	[AccessedThroughProperty("Panel116")]
	private Panel _Panel116;

	[AccessedThroughProperty("Label510")]
	private Label _Label510;

	[AccessedThroughProperty("Label511")]
	private Label _Label511;

	[AccessedThroughProperty("Label513")]
	private Label _Label513;

	[AccessedThroughProperty("Label514")]
	private Label _Label514;

	[AccessedThroughProperty("DomainUpDown34")]
	private DomainUpDown _DomainUpDown34;

	[AccessedThroughProperty("DomainUpDown35")]
	private DomainUpDown _DomainUpDown35;

	[AccessedThroughProperty("DomainUpDown36")]
	private DomainUpDown _DomainUpDown36;

	[AccessedThroughProperty("TrackBar31")]
	private TrackBar _TrackBar31;

	[AccessedThroughProperty("Panel117")]
	private Panel _Panel117;

	[AccessedThroughProperty("Label515")]
	private Label _Label515;

	[AccessedThroughProperty("Label516")]
	private Label _Label516;

	[AccessedThroughProperty("Label517")]
	private Label _Label517;

	[AccessedThroughProperty("Label518")]
	private Label _Label518;

	[AccessedThroughProperty("DomainUpDown37")]
	private DomainUpDown _DomainUpDown37;

	[AccessedThroughProperty("DomainUpDown38")]
	private DomainUpDown _DomainUpDown38;

	[AccessedThroughProperty("DomainUpDown39")]
	private DomainUpDown _DomainUpDown39;

	[AccessedThroughProperty("TrackBar32")]
	private TrackBar _TrackBar32;

	[AccessedThroughProperty("ButtonX3")]
	private ButtonX _ButtonX3;

	[AccessedThroughProperty("Panel118")]
	private Panel _Panel118;

	[AccessedThroughProperty("Label520")]
	private Label _Label520;

	[AccessedThroughProperty("Label521")]
	private Label _Label521;

	[AccessedThroughProperty("Label522")]
	private Label _Label522;

	[AccessedThroughProperty("Label523")]
	private Label _Label523;

	[AccessedThroughProperty("DomainUpDown40")]
	private DomainUpDown _DomainUpDown40;

	[AccessedThroughProperty("DomainUpDown41")]
	private DomainUpDown _DomainUpDown41;

	[AccessedThroughProperty("DomainUpDown42")]
	private DomainUpDown _DomainUpDown42;

	[AccessedThroughProperty("TrackBar33")]
	private TrackBar _TrackBar33;

	[AccessedThroughProperty("ButtonX4")]
	private ButtonX _ButtonX4;

	[AccessedThroughProperty("Panel119")]
	private Panel _Panel119;

	[AccessedThroughProperty("Label524")]
	private Label _Label524;

	[AccessedThroughProperty("Label525")]
	private Label _Label525;

	[AccessedThroughProperty("Label526")]
	private Label _Label526;

	[AccessedThroughProperty("Label527")]
	private Label _Label527;

	[AccessedThroughProperty("DomainUpDown43")]
	private DomainUpDown _DomainUpDown43;

	[AccessedThroughProperty("DomainUpDown44")]
	private DomainUpDown _DomainUpDown44;

	[AccessedThroughProperty("DomainUpDown45")]
	private DomainUpDown _DomainUpDown45;

	[AccessedThroughProperty("TrackBar34")]
	private TrackBar _TrackBar34;

	[AccessedThroughProperty("Panel120")]
	private Panel _Panel120;

	[AccessedThroughProperty("Label528")]
	private Label _Label528;

	[AccessedThroughProperty("Label529")]
	private Label _Label529;

	[AccessedThroughProperty("Label530")]
	private Label _Label530;

	[AccessedThroughProperty("Label532")]
	private Label _Label532;

	[AccessedThroughProperty("DomainUpDown46")]
	private DomainUpDown _DomainUpDown46;

	[AccessedThroughProperty("DomainUpDown47")]
	private DomainUpDown _DomainUpDown47;

	[AccessedThroughProperty("DomainUpDown48")]
	private DomainUpDown _DomainUpDown48;

	[AccessedThroughProperty("TrackBar35")]
	private TrackBar _TrackBar35;

	[AccessedThroughProperty("Panel121")]
	private Panel _Panel121;

	[AccessedThroughProperty("Label533")]
	private Label _Label533;

	[AccessedThroughProperty("Label535")]
	private Label _Label535;

	[AccessedThroughProperty("Label536")]
	private Label _Label536;

	[AccessedThroughProperty("Label537")]
	private Label _Label537;

	[AccessedThroughProperty("DomainUpDown49")]
	private DomainUpDown _DomainUpDown49;

	[AccessedThroughProperty("DomainUpDown50")]
	private DomainUpDown _DomainUpDown50;

	[AccessedThroughProperty("DomainUpDown51")]
	private DomainUpDown _DomainUpDown51;

	[AccessedThroughProperty("TrackBar36")]
	private TrackBar _TrackBar36;

	[AccessedThroughProperty("PictureBox2")]
	private PictureBox _PictureBox2;

	[AccessedThroughProperty("Panel122")]
	private Panel _Panel122;

	[AccessedThroughProperty("Label538")]
	private Label _Label538;

	[AccessedThroughProperty("Label539")]
	private Label _Label539;

	[AccessedThroughProperty("Label541")]
	private Label _Label541;

	[AccessedThroughProperty("Label542")]
	private Label _Label542;

	[AccessedThroughProperty("DomainUpDown52")]
	private DomainUpDown _DomainUpDown52;

	[AccessedThroughProperty("DomainUpDown53")]
	private DomainUpDown _DomainUpDown53;

	[AccessedThroughProperty("DomainUpDown54")]
	private DomainUpDown _DomainUpDown54;

	[AccessedThroughProperty("TrackBar37")]
	private TrackBar _TrackBar37;

	[AccessedThroughProperty("Panel123")]
	private Panel _Panel123;

	[AccessedThroughProperty("Label543")]
	private Label _Label543;

	[AccessedThroughProperty("Label544")]
	private Label _Label544;

	[AccessedThroughProperty("Label545")]
	private Label _Label545;

	[AccessedThroughProperty("Label546")]
	private Label _Label546;

	[AccessedThroughProperty("DomainUpDown55")]
	private DomainUpDown _DomainUpDown55;

	[AccessedThroughProperty("DomainUpDown56")]
	private DomainUpDown _DomainUpDown56;

	[AccessedThroughProperty("DomainUpDown57")]
	private DomainUpDown _DomainUpDown57;

	[AccessedThroughProperty("TrackBar38")]
	private TrackBar _TrackBar38;

	[AccessedThroughProperty("Label547")]
	private Label _Label547;

	[AccessedThroughProperty("Panel124")]
	private Panel _Panel124;

	[AccessedThroughProperty("Label578")]
	private Label _Label578;

	[AccessedThroughProperty("Label579")]
	private Label _Label579;

	[AccessedThroughProperty("Label580")]
	private Label _Label580;

	[AccessedThroughProperty("Label616")]
	private Label _Label616;

	[AccessedThroughProperty("DomainUpDown58")]
	private DomainUpDown _DomainUpDown58;

	[AccessedThroughProperty("DomainUpDown59")]
	private DomainUpDown _DomainUpDown59;

	[AccessedThroughProperty("DomainUpDown60")]
	private DomainUpDown _DomainUpDown60;

	[AccessedThroughProperty("TrackBar39")]
	private TrackBar _TrackBar39;

	[AccessedThroughProperty("Label617")]
	private Label _Label617;

	[AccessedThroughProperty("Label618")]
	private Label _Label618;

	[AccessedThroughProperty("Label619")]
	private Label _Label619;

	[AccessedThroughProperty("SuperTabControlPanel16")]
	private SuperTabControlPanel _SuperTabControlPanel16;

	[AccessedThroughProperty("SuperTabControlPanel20")]
	private SuperTabControlPanel _SuperTabControlPanel20;

	[AccessedThroughProperty("GroupBox16")]
	private GroupBox _GroupBox16;

	[AccessedThroughProperty("ComboBox7")]
	private ComboBox _ComboBox7;

	[AccessedThroughProperty("Label622")]
	private Label _Label622;

	[AccessedThroughProperty("Label623")]
	private Label _Label623;

	[AccessedThroughProperty("Label624")]
	private Label _Label624;

	[AccessedThroughProperty("TrackBar40")]
	private TrackBar _TrackBar40;

	[AccessedThroughProperty("GroupBox17")]
	private GroupBox _GroupBox17;

	[AccessedThroughProperty("TrackBar41")]
	private TrackBar _TrackBar41;

	[AccessedThroughProperty("GroupBox18")]
	private GroupBox _GroupBox18;

	[AccessedThroughProperty("Button3")]
	private Button _Button3;

	[AccessedThroughProperty("Button4")]
	private Button _Button4;

	[AccessedThroughProperty("GroupBox19")]
	private GroupBox _GroupBox19;

	[AccessedThroughProperty("Label625")]
	private Label _Label625;

	[AccessedThroughProperty("Label626")]
	private Label _Label626;

	[AccessedThroughProperty("Label627")]
	private Label _Label627;

	[AccessedThroughProperty("Label628")]
	private Label _Label628;

	[AccessedThroughProperty("TrackBar42")]
	private TrackBar _TrackBar42;

	[AccessedThroughProperty("Label631")]
	private Label _Label631;

	[AccessedThroughProperty("TrackBar43")]
	private TrackBar _TrackBar43;

	[AccessedThroughProperty("TrackBar44")]
	private TrackBar _TrackBar44;

	[AccessedThroughProperty("Label632")]
	private Label _Label632;

	[AccessedThroughProperty("Panel125")]
	private Panel _Panel125;

	[AccessedThroughProperty("Label634")]
	private Label _Label634;

	[AccessedThroughProperty("Label635")]
	private Label _Label635;

	[AccessedThroughProperty("Label636")]
	private Label _Label636;

	[AccessedThroughProperty("Label642")]
	private Label _Label642;

	[AccessedThroughProperty("DomainUpDown61")]
	private DomainUpDown _DomainUpDown61;

	[AccessedThroughProperty("DomainUpDown62")]
	private DomainUpDown _DomainUpDown62;

	[AccessedThroughProperty("DomainUpDown63")]
	private DomainUpDown _DomainUpDown63;

	[AccessedThroughProperty("TrackBar45")]
	private TrackBar _TrackBar45;

	[AccessedThroughProperty("Panel126")]
	private Panel _Panel126;

	[AccessedThroughProperty("Label643")]
	private Label _Label643;

	[AccessedThroughProperty("Label644")]
	private Label _Label644;

	[AccessedThroughProperty("Label646")]
	private Label _Label646;

	[AccessedThroughProperty("Label647")]
	private Label _Label647;

	[AccessedThroughProperty("DomainUpDown64")]
	private DomainUpDown _DomainUpDown64;

	[AccessedThroughProperty("DomainUpDown65")]
	private DomainUpDown _DomainUpDown65;

	[AccessedThroughProperty("DomainUpDown66")]
	private DomainUpDown _DomainUpDown66;

	[AccessedThroughProperty("TrackBar46")]
	private TrackBar _TrackBar46;

	[AccessedThroughProperty("ButtonX5")]
	private ButtonX _ButtonX5;

	[AccessedThroughProperty("Panel127")]
	private Panel _Panel127;

	[AccessedThroughProperty("Label648")]
	private Label _Label648;

	[AccessedThroughProperty("Label649")]
	private Label _Label649;

	[AccessedThroughProperty("Label650")]
	private Label _Label650;

	[AccessedThroughProperty("Label651")]
	private Label _Label651;

	[AccessedThroughProperty("DomainUpDown67")]
	private DomainUpDown _DomainUpDown67;

	[AccessedThroughProperty("DomainUpDown68")]
	private DomainUpDown _DomainUpDown68;

	[AccessedThroughProperty("DomainUpDown69")]
	private DomainUpDown _DomainUpDown69;

	[AccessedThroughProperty("TrackBar47")]
	private TrackBar _TrackBar47;

	[AccessedThroughProperty("ButtonX6")]
	private ButtonX _ButtonX6;

	[AccessedThroughProperty("Panel128")]
	private Panel _Panel128;

	[AccessedThroughProperty("Label654")]
	private Label _Label654;

	[AccessedThroughProperty("Label655")]
	private Label _Label655;

	[AccessedThroughProperty("Label658")]
	private Label _Label658;

	[AccessedThroughProperty("Label659")]
	private Label _Label659;

	[AccessedThroughProperty("DomainUpDown70")]
	private DomainUpDown _DomainUpDown70;

	[AccessedThroughProperty("DomainUpDown71")]
	private DomainUpDown _DomainUpDown71;

	[AccessedThroughProperty("DomainUpDown72")]
	private DomainUpDown _DomainUpDown72;

	[AccessedThroughProperty("TrackBar48")]
	private TrackBar _TrackBar48;

	[AccessedThroughProperty("Panel129")]
	private Panel _Panel129;

	[AccessedThroughProperty("Label660")]
	private Label _Label660;

	[AccessedThroughProperty("Label661")]
	private Label _Label661;

	[AccessedThroughProperty("Label664")]
	private Label _Label664;

	[AccessedThroughProperty("Label665")]
	private Label _Label665;

	[AccessedThroughProperty("DomainUpDown73")]
	private DomainUpDown _DomainUpDown73;

	[AccessedThroughProperty("DomainUpDown74")]
	private DomainUpDown _DomainUpDown74;

	[AccessedThroughProperty("DomainUpDown75")]
	private DomainUpDown _DomainUpDown75;

	[AccessedThroughProperty("TrackBar49")]
	private TrackBar _TrackBar49;

	[AccessedThroughProperty("Panel130")]
	private Panel _Panel130;

	[AccessedThroughProperty("Label666")]
	private Label _Label666;

	[AccessedThroughProperty("Label667")]
	private Label _Label667;

	[AccessedThroughProperty("Label668")]
	private Label _Label668;

	[AccessedThroughProperty("Label669")]
	private Label _Label669;

	[AccessedThroughProperty("DomainUpDown76")]
	private DomainUpDown _DomainUpDown76;

	[AccessedThroughProperty("DomainUpDown77")]
	private DomainUpDown _DomainUpDown77;

	[AccessedThroughProperty("DomainUpDown78")]
	private DomainUpDown _DomainUpDown78;

	[AccessedThroughProperty("TrackBar50")]
	private TrackBar _TrackBar50;

	[AccessedThroughProperty("PictureBox3")]
	private PictureBox _PictureBox3;

	[AccessedThroughProperty("Panel131")]
	private Panel _Panel131;

	[AccessedThroughProperty("Label670")]
	private Label _Label670;

	[AccessedThroughProperty("Label671")]
	private Label _Label671;

	[AccessedThroughProperty("Label672")]
	private Label _Label672;

	[AccessedThroughProperty("Label673")]
	private Label _Label673;

	[AccessedThroughProperty("DomainUpDown79")]
	private DomainUpDown _DomainUpDown79;

	[AccessedThroughProperty("DomainUpDown80")]
	private DomainUpDown _DomainUpDown80;

	[AccessedThroughProperty("DomainUpDown81")]
	private DomainUpDown _DomainUpDown81;

	[AccessedThroughProperty("TrackBar51")]
	private TrackBar _TrackBar51;

	[AccessedThroughProperty("Panel132")]
	private Panel _Panel132;

	[AccessedThroughProperty("Label674")]
	private Label _Label674;

	[AccessedThroughProperty("Label675")]
	private Label _Label675;

	[AccessedThroughProperty("Label676")]
	private Label _Label676;

	[AccessedThroughProperty("Label677")]
	private Label _Label677;

	[AccessedThroughProperty("DomainUpDown82")]
	private DomainUpDown _DomainUpDown82;

	[AccessedThroughProperty("DomainUpDown83")]
	private DomainUpDown _DomainUpDown83;

	[AccessedThroughProperty("DomainUpDown84")]
	private DomainUpDown _DomainUpDown84;

	[AccessedThroughProperty("TrackBar52")]
	private TrackBar _TrackBar52;

	[AccessedThroughProperty("Label678")]
	private Label _Label678;

	[AccessedThroughProperty("Panel133")]
	private Panel _Panel133;

	[AccessedThroughProperty("Label679")]
	private Label _Label679;

	[AccessedThroughProperty("Label680")]
	private Label _Label680;

	[AccessedThroughProperty("Label681")]
	private Label _Label681;

	[AccessedThroughProperty("Label682")]
	private Label _Label682;

	[AccessedThroughProperty("DomainUpDown85")]
	private DomainUpDown _DomainUpDown85;

	[AccessedThroughProperty("DomainUpDown86")]
	private DomainUpDown _DomainUpDown86;

	[AccessedThroughProperty("DomainUpDown87")]
	private DomainUpDown _DomainUpDown87;

	[AccessedThroughProperty("TrackBar53")]
	private TrackBar _TrackBar53;

	[AccessedThroughProperty("Label683")]
	private Label _Label683;

	[AccessedThroughProperty("Label684")]
	private Label _Label684;

	[AccessedThroughProperty("Label685")]
	private Label _Label685;

	[AccessedThroughProperty("SuperTabControlPanel17")]
	private SuperTabControlPanel _SuperTabControlPanel17;

	[AccessedThroughProperty("SuperTabControlPanel21")]
	private SuperTabControlPanel _SuperTabControlPanel21;

	[AccessedThroughProperty("GroupBox20")]
	private GroupBox _GroupBox20;

	[AccessedThroughProperty("ComboBox8")]
	private ComboBox _ComboBox8;

	[AccessedThroughProperty("Label686")]
	private Label _Label686;

	[AccessedThroughProperty("Label687")]
	private Label _Label687;

	[AccessedThroughProperty("Label688")]
	private Label _Label688;

	[AccessedThroughProperty("TrackBar54")]
	private TrackBar _TrackBar54;

	[AccessedThroughProperty("GroupBox21")]
	private GroupBox _GroupBox21;

	[AccessedThroughProperty("TrackBar55")]
	private TrackBar _TrackBar55;

	[AccessedThroughProperty("GroupBox22")]
	private GroupBox _GroupBox22;

	[AccessedThroughProperty("Button5")]
	private Button _Button5;

	[AccessedThroughProperty("Button6")]
	private Button _Button6;

	[AccessedThroughProperty("GroupBox23")]
	private GroupBox _GroupBox23;

	[AccessedThroughProperty("Label689")]
	private Label _Label689;

	[AccessedThroughProperty("Label690")]
	private Label _Label690;

	[AccessedThroughProperty("Label691")]
	private Label _Label691;

	[AccessedThroughProperty("Label692")]
	private Label _Label692;

	[AccessedThroughProperty("TrackBar56")]
	private TrackBar _TrackBar56;

	[AccessedThroughProperty("Label693")]
	private Label _Label693;

	[AccessedThroughProperty("TrackBar57")]
	private TrackBar _TrackBar57;

	[AccessedThroughProperty("TrackBar58")]
	private TrackBar _TrackBar58;

	[AccessedThroughProperty("Label694")]
	private Label _Label694;

	[AccessedThroughProperty("Panel134")]
	private Panel _Panel134;

	[AccessedThroughProperty("Label695")]
	private Label _Label695;

	[AccessedThroughProperty("Label696")]
	private Label _Label696;

	[AccessedThroughProperty("Label697")]
	private Label _Label697;

	[AccessedThroughProperty("Label698")]
	private Label _Label698;

	[AccessedThroughProperty("DomainUpDown88")]
	private DomainUpDown _DomainUpDown88;

	[AccessedThroughProperty("DomainUpDown89")]
	private DomainUpDown _DomainUpDown89;

	[AccessedThroughProperty("DomainUpDown90")]
	private DomainUpDown _DomainUpDown90;

	[AccessedThroughProperty("TrackBar59")]
	private TrackBar _TrackBar59;

	[AccessedThroughProperty("Panel135")]
	private Panel _Panel135;

	[AccessedThroughProperty("Label699")]
	private Label _Label699;

	[AccessedThroughProperty("Label700")]
	private Label _Label700;

	[AccessedThroughProperty("Label701")]
	private Label _Label701;

	[AccessedThroughProperty("Label702")]
	private Label _Label702;

	[AccessedThroughProperty("DomainUpDown91")]
	private DomainUpDown _DomainUpDown91;

	[AccessedThroughProperty("DomainUpDown92")]
	private DomainUpDown _DomainUpDown92;

	[AccessedThroughProperty("DomainUpDown93")]
	private DomainUpDown _DomainUpDown93;

	[AccessedThroughProperty("TrackBar60")]
	private TrackBar _TrackBar60;

	[AccessedThroughProperty("ButtonX7")]
	private ButtonX _ButtonX7;

	[AccessedThroughProperty("Panel136")]
	private Panel _Panel136;

	[AccessedThroughProperty("Label703")]
	private Label _Label703;

	[AccessedThroughProperty("Label704")]
	private Label _Label704;

	[AccessedThroughProperty("Label705")]
	private Label _Label705;

	[AccessedThroughProperty("Label706")]
	private Label _Label706;

	[AccessedThroughProperty("DomainUpDown94")]
	private DomainUpDown _DomainUpDown94;

	[AccessedThroughProperty("DomainUpDown95")]
	private DomainUpDown _DomainUpDown95;

	[AccessedThroughProperty("DomainUpDown96")]
	private DomainUpDown _DomainUpDown96;

	[AccessedThroughProperty("TrackBar61")]
	private TrackBar _TrackBar61;

	[AccessedThroughProperty("ButtonX8")]
	private ButtonX _ButtonX8;

	[AccessedThroughProperty("Panel137")]
	private Panel _Panel137;

	[AccessedThroughProperty("Label707")]
	private Label _Label707;

	[AccessedThroughProperty("Label708")]
	private Label _Label708;

	[AccessedThroughProperty("Label709")]
	private Label _Label709;

	[AccessedThroughProperty("Label710")]
	private Label _Label710;

	[AccessedThroughProperty("DomainUpDown97")]
	private DomainUpDown _DomainUpDown97;

	[AccessedThroughProperty("DomainUpDown98")]
	private DomainUpDown _DomainUpDown98;

	[AccessedThroughProperty("DomainUpDown99")]
	private DomainUpDown _DomainUpDown99;

	[AccessedThroughProperty("TrackBar62")]
	private TrackBar _TrackBar62;

	[AccessedThroughProperty("Panel138")]
	private Panel _Panel138;

	[AccessedThroughProperty("Label711")]
	private Label _Label711;

	[AccessedThroughProperty("Label712")]
	private Label _Label712;

	[AccessedThroughProperty("Label713")]
	private Label _Label713;

	[AccessedThroughProperty("Label714")]
	private Label _Label714;

	[AccessedThroughProperty("DomainUpDown100")]
	private DomainUpDown _DomainUpDown100;

	[AccessedThroughProperty("DomainUpDown101")]
	private DomainUpDown _DomainUpDown101;

	[AccessedThroughProperty("DomainUpDown102")]
	private DomainUpDown _DomainUpDown102;

	[AccessedThroughProperty("TrackBar63")]
	private TrackBar _TrackBar63;

	[AccessedThroughProperty("Panel139")]
	private Panel _Panel139;

	[AccessedThroughProperty("Label715")]
	private Label _Label715;

	[AccessedThroughProperty("Label716")]
	private Label _Label716;

	[AccessedThroughProperty("Label717")]
	private Label _Label717;

	[AccessedThroughProperty("Label718")]
	private Label _Label718;

	[AccessedThroughProperty("DomainUpDown103")]
	private DomainUpDown _DomainUpDown103;

	[AccessedThroughProperty("DomainUpDown104")]
	private DomainUpDown _DomainUpDown104;

	[AccessedThroughProperty("DomainUpDown105")]
	private DomainUpDown _DomainUpDown105;

	[AccessedThroughProperty("TrackBar64")]
	private TrackBar _TrackBar64;

	[AccessedThroughProperty("PictureBox4")]
	private PictureBox _PictureBox4;

	[AccessedThroughProperty("Panel140")]
	private Panel _Panel140;

	[AccessedThroughProperty("Label719")]
	private Label _Label719;

	[AccessedThroughProperty("Label720")]
	private Label _Label720;

	[AccessedThroughProperty("Label721")]
	private Label _Label721;

	[AccessedThroughProperty("Label722")]
	private Label _Label722;

	[AccessedThroughProperty("DomainUpDown106")]
	private DomainUpDown _DomainUpDown106;

	[AccessedThroughProperty("DomainUpDown107")]
	private DomainUpDown _DomainUpDown107;

	[AccessedThroughProperty("DomainUpDown108")]
	private DomainUpDown _DomainUpDown108;

	[AccessedThroughProperty("TrackBar65")]
	private TrackBar _TrackBar65;

	[AccessedThroughProperty("Panel141")]
	private Panel _Panel141;

	[AccessedThroughProperty("Label723")]
	private Label _Label723;

	[AccessedThroughProperty("Label724")]
	private Label _Label724;

	[AccessedThroughProperty("Label725")]
	private Label _Label725;

	[AccessedThroughProperty("Label726")]
	private Label _Label726;

	[AccessedThroughProperty("DomainUpDown109")]
	private DomainUpDown _DomainUpDown109;

	[AccessedThroughProperty("DomainUpDown110")]
	private DomainUpDown _DomainUpDown110;

	[AccessedThroughProperty("DomainUpDown111")]
	private DomainUpDown _DomainUpDown111;

	[AccessedThroughProperty("TrackBar66")]
	private TrackBar _TrackBar66;

	[AccessedThroughProperty("Label727")]
	private Label _Label727;

	[AccessedThroughProperty("Panel142")]
	private Panel _Panel142;

	[AccessedThroughProperty("Label728")]
	private Label _Label728;

	[AccessedThroughProperty("Label729")]
	private Label _Label729;

	[AccessedThroughProperty("Label730")]
	private Label _Label730;

	[AccessedThroughProperty("Label731")]
	private Label _Label731;

	[AccessedThroughProperty("DomainUpDown112")]
	private DomainUpDown _DomainUpDown112;

	[AccessedThroughProperty("DomainUpDown113")]
	private DomainUpDown _DomainUpDown113;

	[AccessedThroughProperty("DomainUpDown114")]
	private DomainUpDown _DomainUpDown114;

	[AccessedThroughProperty("TrackBar67")]
	private TrackBar _TrackBar67;

	[AccessedThroughProperty("Label732")]
	private Label _Label732;

	[AccessedThroughProperty("Label733")]
	private Label _Label733;

	[AccessedThroughProperty("Label734")]
	private Label _Label734;

	[AccessedThroughProperty("SuperTabControlPanel14")]
	private SuperTabControlPanel _SuperTabControlPanel14;

	[AccessedThroughProperty("SuperTabControlPanel18")]
	private SuperTabControlPanel _SuperTabControlPanel18;

	[AccessedThroughProperty("GroupBox9")]
	private GroupBox _GroupBox9;

	[AccessedThroughProperty("Label260")]
	private Label _Label260;

	[AccessedThroughProperty("ComboBox3")]
	private ComboBox _ComboBox3;

	[AccessedThroughProperty("ComboBox5")]
	private ComboBox _ComboBox5;

	[AccessedThroughProperty("Label270")]
	private Label _Label270;

	[AccessedThroughProperty("Label272")]
	private Label _Label272;

	[AccessedThroughProperty("Label277")]
	private Label _Label277;

	[AccessedThroughProperty("Label278")]
	private Label _Label278;

	[AccessedThroughProperty("Label279")]
	private Label _Label279;

	[AccessedThroughProperty("TrackBar10")]
	private TrackBar _TrackBar10;

	[AccessedThroughProperty("TrackBar12")]
	private TrackBar _TrackBar12;

	[AccessedThroughProperty("GroupBox10")]
	private GroupBox _GroupBox10;

	[AccessedThroughProperty("Label349")]
	private Label _Label349;

	[AccessedThroughProperty("Label356")]
	private Label _Label356;

	[AccessedThroughProperty("TrackBar13")]
	private TrackBar _TrackBar13;

	[AccessedThroughProperty("Panel9")]
	private Panel _Panel9;

	[AccessedThroughProperty("Label382")]
	private Label _Label382;

	[AccessedThroughProperty("Label383")]
	private Label _Label383;

	[AccessedThroughProperty("Label388")]
	private Label _Label388;

	[AccessedThroughProperty("Label389")]
	private Label _Label389;

	[AccessedThroughProperty("DomainUpDown7")]
	private DomainUpDown _DomainUpDown7;

	[AccessedThroughProperty("DomainUpDown8")]
	private DomainUpDown _DomainUpDown8;

	[AccessedThroughProperty("DomainUpDown9")]
	private DomainUpDown _DomainUpDown9;

	[AccessedThroughProperty("TrackBar14")]
	private TrackBar _TrackBar14;

	[AccessedThroughProperty("Panel75")]
	private Panel _Panel75;

	[AccessedThroughProperty("Label390")]
	private Label _Label390;

	[AccessedThroughProperty("Label391")]
	private Label _Label391;

	[AccessedThroughProperty("Label393")]
	private Label _Label393;

	[AccessedThroughProperty("Label394")]
	private Label _Label394;

	[AccessedThroughProperty("DomainUpDown10")]
	private DomainUpDown _DomainUpDown10;

	[AccessedThroughProperty("DomainUpDown11")]
	private DomainUpDown _DomainUpDown11;

	[AccessedThroughProperty("DomainUpDown12")]
	private DomainUpDown _DomainUpDown12;

	[AccessedThroughProperty("TrackBar15")]
	private TrackBar _TrackBar15;

	[AccessedThroughProperty("GroupBox11")]
	private GroupBox _GroupBox11;

	[AccessedThroughProperty("Label397")]
	private Label _Label397;

	[AccessedThroughProperty("Label400")]
	private Label _Label400;

	[AccessedThroughProperty("Label401")]
	private Label _Label401;

	[AccessedThroughProperty("Label402")]
	private Label _Label402;

	[AccessedThroughProperty("TrackBar16")]
	private TrackBar _TrackBar16;

	[AccessedThroughProperty("Label403")]
	private Label _Label403;

	[AccessedThroughProperty("TrackBar17")]
	private TrackBar _TrackBar17;

	[AccessedThroughProperty("TrackBar18")]
	private TrackBar _TrackBar18;

	[AccessedThroughProperty("Label404")]
	private Label _Label404;

	[AccessedThroughProperty("ButtonX1")]
	private ButtonX _ButtonX1;

	[AccessedThroughProperty("ButtonX2")]
	private ButtonX _ButtonX2;

	[AccessedThroughProperty("PictureBox1")]
	private PictureBox _PictureBox1;

	[AccessedThroughProperty("Label405")]
	private Label _Label405;

	[AccessedThroughProperty("Label408")]
	private Label _Label408;

	[AccessedThroughProperty("Label409")]
	private Label _Label409;

	[AccessedThroughProperty("Panel89")]
	private Panel _Panel89;

	[AccessedThroughProperty("Label410")]
	private Label _Label410;

	[AccessedThroughProperty("Label413")]
	private Label _Label413;

	[AccessedThroughProperty("Label415")]
	private Label _Label415;

	[AccessedThroughProperty("Label416")]
	private Label _Label416;

	[AccessedThroughProperty("DomainUpDown13")]
	private DomainUpDown _DomainUpDown13;

	[AccessedThroughProperty("DomainUpDown14")]
	private DomainUpDown _DomainUpDown14;

	[AccessedThroughProperty("DomainUpDown15")]
	private DomainUpDown _DomainUpDown15;

	[AccessedThroughProperty("TrackBar19")]
	private TrackBar _TrackBar19;

	[AccessedThroughProperty("Label418")]
	private Label _Label418;

	[AccessedThroughProperty("Panel110")]
	private Panel _Panel110;

	[AccessedThroughProperty("Label420")]
	private Label _Label420;

	[AccessedThroughProperty("Label421")]
	private Label _Label421;

	[AccessedThroughProperty("Label422")]
	private Label _Label422;

	[AccessedThroughProperty("Label423")]
	private Label _Label423;

	[AccessedThroughProperty("DomainUpDown16")]
	private DomainUpDown _DomainUpDown16;

	[AccessedThroughProperty("DomainUpDown17")]
	private DomainUpDown _DomainUpDown17;

	[AccessedThroughProperty("DomainUpDown18")]
	private DomainUpDown _DomainUpDown18;

	[AccessedThroughProperty("TrackBar20")]
	private TrackBar _TrackBar20;

	[AccessedThroughProperty("Panel111")]
	private Panel _Panel111;

	[AccessedThroughProperty("Label424")]
	private Label _Label424;

	[AccessedThroughProperty("Label425")]
	private Label _Label425;

	[AccessedThroughProperty("Label426")]
	private Label _Label426;

	[AccessedThroughProperty("Label427")]
	private Label _Label427;

	[AccessedThroughProperty("DomainUpDown19")]
	private DomainUpDown _DomainUpDown19;

	[AccessedThroughProperty("DomainUpDown20")]
	private DomainUpDown _DomainUpDown20;

	[AccessedThroughProperty("DomainUpDown21")]
	private DomainUpDown _DomainUpDown21;

	[AccessedThroughProperty("TrackBar21")]
	private TrackBar _TrackBar21;

	[AccessedThroughProperty("Panel112")]
	private Panel _Panel112;

	[AccessedThroughProperty("Label428")]
	private Label _Label428;

	[AccessedThroughProperty("Label429")]
	private Label _Label429;

	[AccessedThroughProperty("Label430")]
	private Label _Label430;

	[AccessedThroughProperty("Label441")]
	private Label _Label441;

	[AccessedThroughProperty("DomainUpDown22")]
	private DomainUpDown _DomainUpDown22;

	[AccessedThroughProperty("DomainUpDown23")]
	private DomainUpDown _DomainUpDown23;

	[AccessedThroughProperty("DomainUpDown24")]
	private DomainUpDown _DomainUpDown24;

	[AccessedThroughProperty("TrackBar22")]
	private TrackBar _TrackBar22;

	[AccessedThroughProperty("Panel113")]
	private Panel _Panel113;

	[AccessedThroughProperty("Label464")]
	private Label _Label464;

	[AccessedThroughProperty("Label466")]
	private Label _Label466;

	[AccessedThroughProperty("Label467")]
	private Label _Label467;

	[AccessedThroughProperty("Label468")]
	private Label _Label468;

	[AccessedThroughProperty("DomainUpDown25")]
	private DomainUpDown _DomainUpDown25;

	[AccessedThroughProperty("DomainUpDown26")]
	private DomainUpDown _DomainUpDown26;

	[AccessedThroughProperty("DomainUpDown27")]
	private DomainUpDown _DomainUpDown27;

	[AccessedThroughProperty("TrackBar23")]
	private TrackBar _TrackBar23;

	[AccessedThroughProperty("Panel114")]
	private Panel _Panel114;

	[AccessedThroughProperty("Label470")]
	private Label _Label470;

	[AccessedThroughProperty("Label471")]
	private Label _Label471;

	[AccessedThroughProperty("Label472")]
	private Label _Label472;

	[AccessedThroughProperty("Label473")]
	private Label _Label473;

	[AccessedThroughProperty("DomainUpDown28")]
	private DomainUpDown _DomainUpDown28;

	[AccessedThroughProperty("DomainUpDown29")]
	private DomainUpDown _DomainUpDown29;

	[AccessedThroughProperty("DomainUpDown30")]
	private DomainUpDown _DomainUpDown30;

	[AccessedThroughProperty("TrackBar24")]
	private TrackBar _TrackBar24;

	[AccessedThroughProperty("Panel115")]
	private Panel _Panel115;

	[AccessedThroughProperty("Label474")]
	private Label _Label474;

	[AccessedThroughProperty("Label475")]
	private Label _Label475;

	[AccessedThroughProperty("Label476")]
	private Label _Label476;

	[AccessedThroughProperty("Label477")]
	private Label _Label477;

	[AccessedThroughProperty("DomainUpDown31")]
	private DomainUpDown _DomainUpDown31;

	[AccessedThroughProperty("DomainUpDown32")]
	private DomainUpDown _DomainUpDown32;

	[AccessedThroughProperty("DomainUpDown33")]
	private DomainUpDown _DomainUpDown33;

	[AccessedThroughProperty("TrackBar25")]
	private TrackBar _TrackBar25;

	[AccessedThroughProperty("SuperTabControlPanel25")]
	private SuperTabControlPanel _SuperTabControlPanel25;

	[AccessedThroughProperty("GroupBox28")]
	private GroupBox _GroupBox28;

	[AccessedThroughProperty("Label824")]
	private Label _Label824;

	[AccessedThroughProperty("Label825")]
	private Label _Label825;

	[AccessedThroughProperty("Label828")]
	private Label _Label828;

	[AccessedThroughProperty("Label829")]
	private Label _Label829;

	[AccessedThroughProperty("GroupBox29")]
	private GroupBox _GroupBox29;

	[AccessedThroughProperty("Label831")]
	private Label _Label831;

	[AccessedThroughProperty("GroupBox30")]
	private GroupBox _GroupBox30;

	[AccessedThroughProperty("Label835")]
	private Label _Label835;

	[AccessedThroughProperty("Label836")]
	private Label _Label836;

	[AccessedThroughProperty("Label837")]
	private Label _Label837;

	[AccessedThroughProperty("PictureBox6")]
	private PictureBox _PictureBox6;

	[AccessedThroughProperty("Panel152")]
	private Panel _Panel152;

	[AccessedThroughProperty("Label735")]
	private Label _Label735;

	[AccessedThroughProperty("Label737")]
	private Label _Label737;

	[AccessedThroughProperty("Label739")]
	private Label _Label739;

	[AccessedThroughProperty("Label787")]
	private Label _Label787;

	[AccessedThroughProperty("Panel153")]
	private Panel _Panel153;

	[AccessedThroughProperty("Label788")]
	private Label _Label788;

	[AccessedThroughProperty("Label789")]
	private Label _Label789;

	[AccessedThroughProperty("Label790")]
	private Label _Label790;

	[AccessedThroughProperty("Label791")]
	private Label _Label791;

	[AccessedThroughProperty("Label792")]
	private Label _Label792;

	[AccessedThroughProperty("Label793")]
	private Label _Label793;

	[AccessedThroughProperty("Label794")]
	private Label _Label794;

	[AccessedThroughProperty("Panel154")]
	private Panel _Panel154;

	[AccessedThroughProperty("Label795")]
	private Label _Label795;

	[AccessedThroughProperty("Label796")]
	private Label _Label796;

	[AccessedThroughProperty("Label797")]
	private Label _Label797;

	[AccessedThroughProperty("Label798")]
	private Label _Label798;

	[AccessedThroughProperty("Label799")]
	private Label _Label799;

	[AccessedThroughProperty("Panel155")]
	private Panel _Panel155;

	[AccessedThroughProperty("Label800")]
	private Label _Label800;

	[AccessedThroughProperty("Label801")]
	private Label _Label801;

	[AccessedThroughProperty("Label802")]
	private Label _Label802;

	[AccessedThroughProperty("Label803")]
	private Label _Label803;

	[AccessedThroughProperty("Panel156")]
	private Panel _Panel156;

	[AccessedThroughProperty("Label804")]
	private Label _Label804;

	[AccessedThroughProperty("Label805")]
	private Label _Label805;

	[AccessedThroughProperty("Label806")]
	private Label _Label806;

	[AccessedThroughProperty("Label807")]
	private Label _Label807;

	[AccessedThroughProperty("Panel157")]
	private Panel _Panel157;

	[AccessedThroughProperty("Label808")]
	private Label _Label808;

	[AccessedThroughProperty("Label809")]
	private Label _Label809;

	[AccessedThroughProperty("Label810")]
	private Label _Label810;

	[AccessedThroughProperty("Label811")]
	private Label _Label811;

	[AccessedThroughProperty("Panel158")]
	private Panel _Panel158;

	[AccessedThroughProperty("Label812")]
	private Label _Label812;

	[AccessedThroughProperty("Label813")]
	private Label _Label813;

	[AccessedThroughProperty("Label814")]
	private Label _Label814;

	[AccessedThroughProperty("Label815")]
	private Label _Label815;

	[AccessedThroughProperty("Panel159")]
	private Panel _Panel159;

	[AccessedThroughProperty("Label816")]
	private Label _Label816;

	[AccessedThroughProperty("Label817")]
	private Label _Label817;

	[AccessedThroughProperty("Label818")]
	private Label _Label818;

	[AccessedThroughProperty("Label819")]
	private Label _Label819;

	[AccessedThroughProperty("Panel160")]
	private Panel _Panel160;

	[AccessedThroughProperty("Label820")]
	private Label _Label820;

	[AccessedThroughProperty("Label821")]
	private Label _Label821;

	[AccessedThroughProperty("Label822")]
	private Label _Label822;

	[AccessedThroughProperty("Label823")]
	private Label _Label823;

	[AccessedThroughProperty("SuperTabControlPanel24")]
	private SuperTabControlPanel _SuperTabControlPanel24;

	[AccessedThroughProperty("SuperTabControlPanel23")]
	private SuperTabControlPanel _SuperTabControlPanel23;

	[AccessedThroughProperty("OutPic")]
	private PictureBox _OutPic;

	[AccessedThroughProperty("GroupBox26")]
	private GroupBox _GroupBox26;

	[AccessedThroughProperty("GroupBox27")]
	private GroupBox _GroupBox27;

	[AccessedThroughProperty("Label784")]
	private Label _Label784;

	[AccessedThroughProperty("Label785")]
	private Label _Label785;

	[AccessedThroughProperty("Label786")]
	private Label _Label786;

	[AccessedThroughProperty("Panel143")]
	private Panel _Panel143;

	[AccessedThroughProperty("Label741")]
	private Label _Label741;

	[AccessedThroughProperty("Label742")]
	private Label _Label742;

	[AccessedThroughProperty("Label743")]
	private Label _Label743;

	[AccessedThroughProperty("Label744")]
	private Label _Label744;

	[AccessedThroughProperty("Panel144")]
	private Panel _Panel144;

	[AccessedThroughProperty("Label745")]
	private Label _Label745;

	[AccessedThroughProperty("Label746")]
	private Label _Label746;

	[AccessedThroughProperty("Label747")]
	private Label _Label747;

	[AccessedThroughProperty("Label748")]
	private Label _Label748;

	[AccessedThroughProperty("Panel147")]
	private Panel _Panel147;

	[AccessedThroughProperty("Label757")]
	private Label _Label757;

	[AccessedThroughProperty("Label758")]
	private Label _Label758;

	[AccessedThroughProperty("Label759")]
	private Label _Label759;

	[AccessedThroughProperty("Label760")]
	private Label _Label760;

	[AccessedThroughProperty("Panel148")]
	private Panel _Panel148;

	[AccessedThroughProperty("Label761")]
	private Label _Label761;

	[AccessedThroughProperty("Label762")]
	private Label _Label762;

	[AccessedThroughProperty("Label763")]
	private Label _Label763;

	[AccessedThroughProperty("Label764")]
	private Label _Label764;

	[AccessedThroughProperty("Label773")]
	private Label _Label773;

	[AccessedThroughProperty("Panel151")]
	private Panel _Panel151;

	[AccessedThroughProperty("Label774")]
	private Label _Label774;

	[AccessedThroughProperty("Label775")]
	private Label _Label775;

	[AccessedThroughProperty("Label776")]
	private Label _Label776;

	[AccessedThroughProperty("Label777")]
	private Label _Label777;

	[AccessedThroughProperty("Label779")]
	private Label _Label779;

	[AccessedThroughProperty("Label780")]
	private Label _Label780;

	[AccessedThroughProperty("GroupBox24")]
	private GroupBox _GroupBox24;

	[AccessedThroughProperty("Label736")]
	private Label _Label736;

	[AccessedThroughProperty("Label740")]
	private Label _Label740;

	[AccessedThroughProperty("SuperTabItem5")]
	private SuperTabItem _SuperTabItem5;

	[AccessedThroughProperty("GroupBox33")]
	private GroupBox _GroupBox33;

	[AccessedThroughProperty("GroupBox34")]
	private GroupBox _GroupBox34;

	[AccessedThroughProperty("Label844")]
	private Label _Label844;

	[AccessedThroughProperty("Label845")]
	private Label _Label845;

	[AccessedThroughProperty("Label846")]
	private Label _Label846;

	[AccessedThroughProperty("Panel161")]
	private Panel _Panel161;

	[AccessedThroughProperty("Label847")]
	private Label _Label847;

	[AccessedThroughProperty("Label848")]
	private Label _Label848;

	[AccessedThroughProperty("Label849")]
	private Label _Label849;

	[AccessedThroughProperty("Label850")]
	private Label _Label850;

	[AccessedThroughProperty("Panel162")]
	private Panel _Panel162;

	[AccessedThroughProperty("Label851")]
	private Label _Label851;

	[AccessedThroughProperty("Label852")]
	private Label _Label852;

	[AccessedThroughProperty("Label853")]
	private Label _Label853;

	[AccessedThroughProperty("Label854")]
	private Label _Label854;

	[AccessedThroughProperty("Panel163")]
	private Panel _Panel163;

	[AccessedThroughProperty("Label855")]
	private Label _Label855;

	[AccessedThroughProperty("Label856")]
	private Label _Label856;

	[AccessedThroughProperty("Label857")]
	private Label _Label857;

	[AccessedThroughProperty("Label858")]
	private Label _Label858;

	[AccessedThroughProperty("Panel164")]
	private Panel _Panel164;

	[AccessedThroughProperty("Label859")]
	private Label _Label859;

	[AccessedThroughProperty("Label860")]
	private Label _Label860;

	[AccessedThroughProperty("Label861")]
	private Label _Label861;

	[AccessedThroughProperty("Label862")]
	private Label _Label862;

	[AccessedThroughProperty("Panel165")]
	private Panel _Panel165;

	[AccessedThroughProperty("Label863")]
	private Label _Label863;

	[AccessedThroughProperty("Label864")]
	private Label _Label864;

	[AccessedThroughProperty("Label865")]
	private Label _Label865;

	[AccessedThroughProperty("Label866")]
	private Label _Label866;

	[AccessedThroughProperty("Panel166")]
	private Panel _Panel166;

	[AccessedThroughProperty("Label867")]
	private Label _Label867;

	[AccessedThroughProperty("Label868")]
	private Label _Label868;

	[AccessedThroughProperty("Label869")]
	private Label _Label869;

	[AccessedThroughProperty("Label870")]
	private Label _Label870;

	[AccessedThroughProperty("PictureBox7")]
	private PictureBox _PictureBox7;

	[AccessedThroughProperty("Panel167")]
	private Panel _Panel167;

	[AccessedThroughProperty("Label871")]
	private Label _Label871;

	[AccessedThroughProperty("Label872")]
	private Label _Label872;

	[AccessedThroughProperty("Label873")]
	private Label _Label873;

	[AccessedThroughProperty("Label874")]
	private Label _Label874;

	[AccessedThroughProperty("Panel168")]
	private Panel _Panel168;

	[AccessedThroughProperty("Label875")]
	private Label _Label875;

	[AccessedThroughProperty("Label876")]
	private Label _Label876;

	[AccessedThroughProperty("Label877")]
	private Label _Label877;

	[AccessedThroughProperty("Label878")]
	private Label _Label878;

	[AccessedThroughProperty("Label879")]
	private Label _Label879;

	[AccessedThroughProperty("Panel169")]
	private Panel _Panel169;

	[AccessedThroughProperty("Label880")]
	private Label _Label880;

	[AccessedThroughProperty("Label881")]
	private Label _Label881;

	[AccessedThroughProperty("Label882")]
	private Label _Label882;

	[AccessedThroughProperty("Label883")]
	private Label _Label883;

	[AccessedThroughProperty("Label884")]
	private Label _Label884;

	[AccessedThroughProperty("Label885")]
	private Label _Label885;

	[AccessedThroughProperty("Label886")]
	private Label _Label886;

	[AccessedThroughProperty("GroupBox35")]
	private GroupBox _GroupBox35;

	[AccessedThroughProperty("Label887")]
	private Label _Label887;

	[AccessedThroughProperty("Label889")]
	private Label _Label889;

	[AccessedThroughProperty("GroupBox37")]
	private GroupBox _GroupBox37;

	[AccessedThroughProperty("GroupBox42")]
	private GroupBox _GroupBox42;

	[AccessedThroughProperty("Label893")]
	private Label _Label893;

	[AccessedThroughProperty("Label894")]
	private Label _Label894;

	[AccessedThroughProperty("Label895")]
	private Label _Label895;

	[AccessedThroughProperty("Panel170")]
	private Panel _Panel170;

	[AccessedThroughProperty("Label896")]
	private Label _Label896;

	[AccessedThroughProperty("Label897")]
	private Label _Label897;

	[AccessedThroughProperty("Label898")]
	private Label _Label898;

	[AccessedThroughProperty("Label899")]
	private Label _Label899;

	[AccessedThroughProperty("Panel171")]
	private Panel _Panel171;

	[AccessedThroughProperty("Label900")]
	private Label _Label900;

	[AccessedThroughProperty("Label901")]
	private Label _Label901;

	[AccessedThroughProperty("Label902")]
	private Label _Label902;

	[AccessedThroughProperty("Label903")]
	private Label _Label903;

	[AccessedThroughProperty("Panel172")]
	private Panel _Panel172;

	[AccessedThroughProperty("Label904")]
	private Label _Label904;

	[AccessedThroughProperty("Label905")]
	private Label _Label905;

	[AccessedThroughProperty("Label906")]
	private Label _Label906;

	[AccessedThroughProperty("Label907")]
	private Label _Label907;

	[AccessedThroughProperty("Panel173")]
	private Panel _Panel173;

	[AccessedThroughProperty("Label908")]
	private Label _Label908;

	[AccessedThroughProperty("Label909")]
	private Label _Label909;

	[AccessedThroughProperty("Label910")]
	private Label _Label910;

	[AccessedThroughProperty("Label911")]
	private Label _Label911;

	[AccessedThroughProperty("Panel174")]
	private Panel _Panel174;

	[AccessedThroughProperty("Label912")]
	private Label _Label912;

	[AccessedThroughProperty("Label913")]
	private Label _Label913;

	[AccessedThroughProperty("Label914")]
	private Label _Label914;

	[AccessedThroughProperty("Label915")]
	private Label _Label915;

	[AccessedThroughProperty("Panel175")]
	private Panel _Panel175;

	[AccessedThroughProperty("Label916")]
	private Label _Label916;

	[AccessedThroughProperty("Label917")]
	private Label _Label917;

	[AccessedThroughProperty("Label918")]
	private Label _Label918;

	[AccessedThroughProperty("Label919")]
	private Label _Label919;

	[AccessedThroughProperty("PictureBox8")]
	private PictureBox _PictureBox8;

	[AccessedThroughProperty("Panel176")]
	private Panel _Panel176;

	[AccessedThroughProperty("Label920")]
	private Label _Label920;

	[AccessedThroughProperty("Label921")]
	private Label _Label921;

	[AccessedThroughProperty("Label922")]
	private Label _Label922;

	[AccessedThroughProperty("Label923")]
	private Label _Label923;

	[AccessedThroughProperty("Panel177")]
	private Panel _Panel177;

	[AccessedThroughProperty("Label924")]
	private Label _Label924;

	[AccessedThroughProperty("Label925")]
	private Label _Label925;

	[AccessedThroughProperty("Label926")]
	private Label _Label926;

	[AccessedThroughProperty("Label927")]
	private Label _Label927;

	[AccessedThroughProperty("Label928")]
	private Label _Label928;

	[AccessedThroughProperty("Panel178")]
	private Panel _Panel178;

	[AccessedThroughProperty("Label929")]
	private Label _Label929;

	[AccessedThroughProperty("Label930")]
	private Label _Label930;

	[AccessedThroughProperty("Label931")]
	private Label _Label931;

	[AccessedThroughProperty("Label932")]
	private Label _Label932;

	[AccessedThroughProperty("Label933")]
	private Label _Label933;

	[AccessedThroughProperty("Label934")]
	private Label _Label934;

	[AccessedThroughProperty("Label935")]
	private Label _Label935;

	[AccessedThroughProperty("LPF_INBT")]
	private ComboBox _LPF_INBT;

	[AccessedThroughProperty("HPF_INBT")]
	private ComboBox _HPF_INBT;

	[AccessedThroughProperty("LPF_INBL")]
	private Label _LPF_INBL;

	[AccessedThroughProperty("HPF_INBL")]
	private Label _HPF_INBL;

	[AccessedThroughProperty("LPF_INB")]
	private TrackBar _LPF_INB;

	[AccessedThroughProperty("HPF_INB")]
	private TrackBar _HPF_INB;

	[AccessedThroughProperty("DELAY_INBL")]
	private Label _DELAY_INBL;

	[AccessedThroughProperty("DELAY_INB")]
	private TrackBar _DELAY_INB;

	[AccessedThroughProperty("DECAY_INBL")]
	private Label _DECAY_INBL;

	[AccessedThroughProperty("AT_INBL")]
	private Label _AT_INBL;

	[AccessedThroughProperty("TH_INBL")]
	private Label _TH_INBL;

	[AccessedThroughProperty("DECAY_INB")]
	private TrackBar _DECAY_INB;

	[AccessedThroughProperty("AT_INB")]
	private TrackBar _AT_INB;

	[AccessedThroughProperty("TH_INB")]
	private TrackBar _TH_INB;

	[AccessedThroughProperty("FREQ_INBTB8")]
	private DomainUpDown _FREQ_INBTB8;

	[AccessedThroughProperty("GAIN_INB8")]
	private DomainUpDown _GAIN_INB8;

	[AccessedThroughProperty("Q_INBTB8")]
	private DomainUpDown _Q_INBTB8;

	[AccessedThroughProperty("GAIN_INBTB8")]
	private TrackBar _GAIN_INBTB8;

	[AccessedThroughProperty("FREQ_INBTB9")]
	private DomainUpDown _FREQ_INBTB9;

	[AccessedThroughProperty("GAIN_INB9")]
	private DomainUpDown _GAIN_INB9;

	[AccessedThroughProperty("Q_INBTB9")]
	private DomainUpDown _Q_INBTB9;

	[AccessedThroughProperty("GAIN_INBTB9")]
	private TrackBar _GAIN_INBTB9;

	[AccessedThroughProperty("EQ_INB_BYPASS")]
	private ButtonX _EQ_INB_BYPASS;

	[AccessedThroughProperty("EQ_INB_RESET")]
	private ButtonX _EQ_INB_RESET;

	[AccessedThroughProperty("FREQ_INBTB5")]
	private DomainUpDown _FREQ_INBTB5;

	[AccessedThroughProperty("GAIN_INB5")]
	private DomainUpDown _GAIN_INB5;

	[AccessedThroughProperty("Q_INBTB5")]
	private DomainUpDown _Q_INBTB5;

	[AccessedThroughProperty("GAIN_INBTB5")]
	private TrackBar _GAIN_INBTB5;

	[AccessedThroughProperty("FREQ_INBTB2")]
	private DomainUpDown _FREQ_INBTB2;

	[AccessedThroughProperty("GAIN_INB2")]
	private DomainUpDown _GAIN_INB2;

	[AccessedThroughProperty("Q_INBTB2")]
	private DomainUpDown _Q_INBTB2;

	[AccessedThroughProperty("GAIN_INBTB2")]
	private TrackBar _GAIN_INBTB2;

	[AccessedThroughProperty("FREQ_INBTB6")]
	private DomainUpDown _FREQ_INBTB6;

	[AccessedThroughProperty("GAIN_INB6")]
	private DomainUpDown _GAIN_INB6;

	[AccessedThroughProperty("Q_INBTB6")]
	private DomainUpDown _Q_INBTB6;

	[AccessedThroughProperty("GAIN_INBTB6")]
	private TrackBar _GAIN_INBTB6;

	[AccessedThroughProperty("FREQ_INBTB1")]
	private DomainUpDown _FREQ_INBTB1;

	[AccessedThroughProperty("GAIN_INB1")]
	private DomainUpDown _GAIN_INB1;

	[AccessedThroughProperty("Q_INBTB1")]
	private DomainUpDown _Q_INBTB1;

	[AccessedThroughProperty("GAIN_INBTB1")]
	private TrackBar _GAIN_INBTB1;

	[AccessedThroughProperty("FREQ_INBTB3")]
	private DomainUpDown _FREQ_INBTB3;

	[AccessedThroughProperty("GAIN_INB3")]
	private DomainUpDown _GAIN_INB3;

	[AccessedThroughProperty("Q_INBTB3")]
	private DomainUpDown _Q_INBTB3;

	[AccessedThroughProperty("GAIN_INBTB3")]
	private TrackBar _GAIN_INBTB3;

	[AccessedThroughProperty("FREQ_INBTB7")]
	private DomainUpDown _FREQ_INBTB7;

	[AccessedThroughProperty("GAIN_INB7")]
	private DomainUpDown _GAIN_INB7;

	[AccessedThroughProperty("Q_INBTB7")]
	private DomainUpDown _Q_INBTB7;

	[AccessedThroughProperty("GAIN_INBTB7")]
	private TrackBar _GAIN_INBTB7;

	[AccessedThroughProperty("FREQ_INBTB4")]
	private DomainUpDown _FREQ_INBTB4;

	[AccessedThroughProperty("GAIN_INB4")]
	private DomainUpDown _GAIN_INB4;

	[AccessedThroughProperty("Q_INBTB4")]
	private DomainUpDown _Q_INBTB4;

	[AccessedThroughProperty("GAIN_INBTB4")]
	private TrackBar _GAIN_INBTB4;

	[AccessedThroughProperty("GroupBox25")]
	private GroupBox _GroupBox25;

	[AccessedThroughProperty("GroupBox31")]
	private GroupBox _GroupBox31;

	[AccessedThroughProperty("OutPhase")]
	private Button _OutPhase;

	[AccessedThroughProperty("OutMute")]
	private Button _OutMute;

	[AccessedThroughProperty("DECAY_OUT2L")]
	private Label _DECAY_OUT2L;

	[AccessedThroughProperty("AT_OUT2L")]
	private Label _AT_OUT2L;

	[AccessedThroughProperty("TH_OUT2L")]
	private Label _TH_OUT2L;

	[AccessedThroughProperty("DECAY_OUT2")]
	private TrackBar _DECAY_OUT2;

	[AccessedThroughProperty("AT_OUT2")]
	private TrackBar _AT_OUT2;

	[AccessedThroughProperty("TH_OUT2")]
	private TrackBar _TH_OUT2;

	[AccessedThroughProperty("FREQ_OUT25")]
	private DomainUpDown _FREQ_OUT25;

	[AccessedThroughProperty("GAIN_OUT25")]
	private DomainUpDown _GAIN_OUT25;

	[AccessedThroughProperty("Q_OUT25")]
	private DomainUpDown _Q_OUT25;

	[AccessedThroughProperty("GAIN_OUTTB25")]
	private TrackBar _GAIN_OUTTB25;

	[AccessedThroughProperty("FREQ_OUT22")]
	private DomainUpDown _FREQ_OUT22;

	[AccessedThroughProperty("GAIN_OUT22")]
	private DomainUpDown _GAIN_OUT22;

	[AccessedThroughProperty("Q_OUT22")]
	private DomainUpDown _Q_OUT22;

	[AccessedThroughProperty("GAIN_OUTTB22")]
	private TrackBar _GAIN_OUTTB22;

	[AccessedThroughProperty("BYPASS_OUT2")]
	private ButtonX _BYPASS_OUT2;

	[AccessedThroughProperty("RESET_OUT2")]
	private ButtonX _RESET_OUT2;

	[AccessedThroughProperty("FREQ_OUT21")]
	private DomainUpDown _FREQ_OUT21;

	[AccessedThroughProperty("GAIN_OUT21")]
	private DomainUpDown _GAIN_OUT21;

	[AccessedThroughProperty("Q_OUT21")]
	private DomainUpDown _Q_OUT21;

	[AccessedThroughProperty("GAIN_OUTTB21")]
	private TrackBar _GAIN_OUTTB21;

	[AccessedThroughProperty("FREQ_OUT23")]
	private DomainUpDown _FREQ_OUT23;

	[AccessedThroughProperty("GAIN_OUT23")]
	private DomainUpDown _GAIN_OUT23;

	[AccessedThroughProperty("Q_OUT23")]
	private DomainUpDown _Q_OUT23;

	[AccessedThroughProperty("GAIN_OUTTB23")]
	private TrackBar _GAIN_OUTTB23;

	[AccessedThroughProperty("FREQ_OUT24")]
	private DomainUpDown _FREQ_OUT24;

	[AccessedThroughProperty("GAIN_OUT24")]
	private DomainUpDown _GAIN_OUT24;

	[AccessedThroughProperty("Q_OUT24")]
	private DomainUpDown _Q_OUT24;

	[AccessedThroughProperty("GAIN_OUTTB24")]
	private TrackBar _GAIN_OUTTB24;

	[AccessedThroughProperty("HPF_OUT2T")]
	private ComboBox _HPF_OUT2T;

	[AccessedThroughProperty("HPF_OUT2L")]
	private Label _HPF_OUT2L;

	[AccessedThroughProperty("HPF_OUT2")]
	private TrackBar _HPF_OUT2;

	[AccessedThroughProperty("VOL_OUT3L")]
	private Label _VOL_OUT3L;

	[AccessedThroughProperty("VOL_OUT3")]
	private TrackBar _VOL_OUT3;

	[AccessedThroughProperty("PHASE_OUT3")]
	private Button _PHASE_OUT3;

	[AccessedThroughProperty("MUTE_OUT3")]
	private Button _MUTE_OUT3;

	[AccessedThroughProperty("DECAY_OUT3L")]
	private Label _DECAY_OUT3L;

	[AccessedThroughProperty("AT_OUT3L")]
	private Label _AT_OUT3L;

	[AccessedThroughProperty("TH_OUT3L")]
	private Label _TH_OUT3L;

	[AccessedThroughProperty("DECAY_OUT3")]
	private TrackBar _DECAY_OUT3;

	[AccessedThroughProperty("AT_OUT3")]
	private TrackBar _AT_OUT3;

	[AccessedThroughProperty("TH_OUT3")]
	private TrackBar _TH_OUT3;

	[AccessedThroughProperty("VOL_OUT4L")]
	private Label _VOL_OUT4L;

	[AccessedThroughProperty("VOL_OUT4")]
	private TrackBar _VOL_OUT4;

	[AccessedThroughProperty("HPF_OUT4T")]
	private ComboBox _HPF_OUT4T;

	[AccessedThroughProperty("HPF_OUT4L")]
	private Label _HPF_OUT4L;

	[AccessedThroughProperty("HPF_OUT4")]
	private TrackBar _HPF_OUT4;

	[AccessedThroughProperty("PHASE_OUT4")]
	private Button _PHASE_OUT4;

	[AccessedThroughProperty("MUTE_OUT4")]
	private Button _MUTE_OUT4;

	[AccessedThroughProperty("DECAY_OUT4L")]
	private Label _DECAY_OUT4L;

	[AccessedThroughProperty("AT_OUT4L")]
	private Label _AT_OUT4L;

	[AccessedThroughProperty("TH_OUT4L")]
	private Label _TH_OUT4L;

	[AccessedThroughProperty("DECAY_OUT4")]
	private TrackBar _DECAY_OUT4;

	[AccessedThroughProperty("AT_OUT4")]
	private TrackBar _AT_OUT4;

	[AccessedThroughProperty("TH_OUT4")]
	private TrackBar _TH_OUT4;

	[AccessedThroughProperty("FREQ_OUT35")]
	private DomainUpDown _FREQ_OUT35;

	[AccessedThroughProperty("GAIN_OUT35")]
	private DomainUpDown _GAIN_OUT35;

	[AccessedThroughProperty("Q_OUT35")]
	private DomainUpDown _Q_OUT35;

	[AccessedThroughProperty("GAIN_OUTTB35")]
	private TrackBar _GAIN_OUTTB35;

	[AccessedThroughProperty("FREQ_OUT32")]
	private DomainUpDown _FREQ_OUT32;

	[AccessedThroughProperty("GAIN_OUT32")]
	private DomainUpDown _GAIN_OUT32;

	[AccessedThroughProperty("Q_OUT32")]
	private DomainUpDown _Q_OUT32;

	[AccessedThroughProperty("GAIN_OUTTB32")]
	private TrackBar _GAIN_OUTTB32;

	[AccessedThroughProperty("BYPASS_OUT3")]
	private ButtonX _BYPASS_OUT3;

	[AccessedThroughProperty("FREQ_OUT38")]
	private DomainUpDown _FREQ_OUT38;

	[AccessedThroughProperty("GAIN_OUT38")]
	private DomainUpDown _GAIN_OUT38;

	[AccessedThroughProperty("Q_OUT38")]
	private DomainUpDown _Q_OUT38;

	[AccessedThroughProperty("GAIN_OUTTB38")]
	private TrackBar _GAIN_OUTTB38;

	[AccessedThroughProperty("RESET_OUT3")]
	private ButtonX _RESET_OUT3;

	[AccessedThroughProperty("FREQ_OUT36")]
	private DomainUpDown _FREQ_OUT36;

	[AccessedThroughProperty("GAIN_OUT36")]
	private DomainUpDown _GAIN_OUT36;

	[AccessedThroughProperty("Q_OUT36")]
	private DomainUpDown _Q_OUT36;

	[AccessedThroughProperty("GAIN_OUTTB36")]
	private TrackBar _GAIN_OUTTB36;

	[AccessedThroughProperty("FREQ_OUT31")]
	private DomainUpDown _FREQ_OUT31;

	[AccessedThroughProperty("GAIN_OUT31")]
	private DomainUpDown _GAIN_OUT31;

	[AccessedThroughProperty("Q_OUT31")]
	private DomainUpDown _Q_OUT31;

	[AccessedThroughProperty("GAIN_OUTTB31")]
	private TrackBar _GAIN_OUTTB31;

	[AccessedThroughProperty("FREQ_OUT33")]
	private DomainUpDown _FREQ_OUT33;

	[AccessedThroughProperty("GAIN_OUT33")]
	private DomainUpDown _GAIN_OUT33;

	[AccessedThroughProperty("Q_OUT33")]
	private DomainUpDown _Q_OUT33;

	[AccessedThroughProperty("GAIN_OUTTB33")]
	private TrackBar _GAIN_OUTTB33;

	[AccessedThroughProperty("FREQ_OUT37")]
	private DomainUpDown _FREQ_OUT37;

	[AccessedThroughProperty("GAIN_OUT37")]
	private DomainUpDown _GAIN_OUT37;

	[AccessedThroughProperty("Q_OUT37")]
	private DomainUpDown _Q_OUT37;

	[AccessedThroughProperty("GAIN_OUTTB37")]
	private TrackBar _GAIN_OUTTB37;

	[AccessedThroughProperty("FREQ_OUT39")]
	private DomainUpDown _FREQ_OUT39;

	[AccessedThroughProperty("GAIN_OUT39")]
	private DomainUpDown _GAIN_OUT39;

	[AccessedThroughProperty("Q_OUT39")]
	private DomainUpDown _Q_OUT39;

	[AccessedThroughProperty("GAIN_OUTTB39")]
	private TrackBar _GAIN_OUTTB39;

	[AccessedThroughProperty("FREQ_OUT34")]
	private DomainUpDown _FREQ_OUT34;

	[AccessedThroughProperty("GAIN_OUT34")]
	private DomainUpDown _GAIN_OUT34;

	[AccessedThroughProperty("Q_OUT34")]
	private DomainUpDown _Q_OUT34;

	[AccessedThroughProperty("GAIN_OUTTB34")]
	private TrackBar _GAIN_OUTTB34;

	[AccessedThroughProperty("FREQ_OUT45")]
	private DomainUpDown _FREQ_OUT45;

	[AccessedThroughProperty("GAIN_OUT45")]
	private DomainUpDown _GAIN_OUT45;

	[AccessedThroughProperty("Q_OUT45")]
	private DomainUpDown _Q_OUT45;

	[AccessedThroughProperty("GAIN_OUTTB45")]
	private TrackBar _GAIN_OUTTB45;

	[AccessedThroughProperty("FREQ_OUT42")]
	private DomainUpDown _FREQ_OUT42;

	[AccessedThroughProperty("GAIN_OUT42")]
	private DomainUpDown _GAIN_OUT42;

	[AccessedThroughProperty("Q_OUT42")]
	private DomainUpDown _Q_OUT42;

	[AccessedThroughProperty("GAIN_OUTTB42")]
	private TrackBar _GAIN_OUTTB42;

	[AccessedThroughProperty("FREQ_OUT48")]
	private DomainUpDown _FREQ_OUT48;

	[AccessedThroughProperty("GAIN_OUT48")]
	private DomainUpDown _GAIN_OUT48;

	[AccessedThroughProperty("Q_OUT48")]
	private DomainUpDown _Q_OUT48;

	[AccessedThroughProperty("GAIN_OUTTB48")]
	private TrackBar _GAIN_OUTTB48;

	[AccessedThroughProperty("FREQ_OUT46")]
	private DomainUpDown _FREQ_OUT46;

	[AccessedThroughProperty("GAIN_OUT46")]
	private DomainUpDown _GAIN_OUT46;

	[AccessedThroughProperty("Q_OUT46")]
	private DomainUpDown _Q_OUT46;

	[AccessedThroughProperty("GAIN_OUTTB46")]
	private TrackBar _GAIN_OUTTB46;

	[AccessedThroughProperty("FREQ_OUT41")]
	private DomainUpDown _FREQ_OUT41;

	[AccessedThroughProperty("GAIN_OUT41")]
	private DomainUpDown _GAIN_OUT41;

	[AccessedThroughProperty("Q_OUT41")]
	private DomainUpDown _Q_OUT41;

	[AccessedThroughProperty("GAIN_OUTTB41")]
	private TrackBar _GAIN_OUTTB41;

	[AccessedThroughProperty("FREQ_OUT43")]
	private DomainUpDown _FREQ_OUT43;

	[AccessedThroughProperty("GAIN_OUT43")]
	private DomainUpDown _GAIN_OUT43;

	[AccessedThroughProperty("Q_OUT43")]
	private DomainUpDown _Q_OUT43;

	[AccessedThroughProperty("GAIN_OUTTB43")]
	private TrackBar _GAIN_OUTTB43;

	[AccessedThroughProperty("FREQ_OUT47")]
	private DomainUpDown _FREQ_OUT47;

	[AccessedThroughProperty("GAIN_OUT47")]
	private DomainUpDown _GAIN_OUT47;

	[AccessedThroughProperty("Q_OUT47")]
	private DomainUpDown _Q_OUT47;

	[AccessedThroughProperty("GAIN_OUTTB47")]
	private TrackBar _GAIN_OUTTB47;

	[AccessedThroughProperty("FREQ_OUT49")]
	private DomainUpDown _FREQ_OUT49;

	[AccessedThroughProperty("GAIN_OUT49")]
	private DomainUpDown _GAIN_OUT49;

	[AccessedThroughProperty("Q_OUT49")]
	private DomainUpDown _Q_OUT49;

	[AccessedThroughProperty("GAIN_OUTTB49")]
	private TrackBar _GAIN_OUTTB49;

	[AccessedThroughProperty("FREQ_OUT44")]
	private DomainUpDown _FREQ_OUT44;

	[AccessedThroughProperty("GAIN_OUT44")]
	private DomainUpDown _GAIN_OUT44;

	[AccessedThroughProperty("Q_OUT44")]
	private DomainUpDown _Q_OUT44;

	[AccessedThroughProperty("GAIN_OUTTB44")]
	private TrackBar _GAIN_OUTTB44;

	[AccessedThroughProperty("BYPASS_OUT4")]
	private ButtonX _BYPASS_OUT4;

	[AccessedThroughProperty("RESET_OUT4")]
	private ButtonX _RESET_OUT4;

	[AccessedThroughProperty("Label261")]
	private Label _Label261;

	[AccessedThroughProperty("LPF_INAT")]
	private ComboBox _LPF_INAT;

	[AccessedThroughProperty("LPF_INAL")]
	private Label _LPF_INAL;

	[AccessedThroughProperty("Label839")]
	private Label _Label839;

	[AccessedThroughProperty("LPF_INA")]
	private TrackBar _LPF_INA;

	[AccessedThroughProperty("GroupBox1")]
	private GroupBox _GroupBox1;

	[AccessedThroughProperty("OutGainT")]
	private Label _OutGainT;

	[AccessedThroughProperty("Label110")]
	private Label _Label110;

	[AccessedThroughProperty("OutGain")]
	private TrackBar _OutGain;

	[AccessedThroughProperty("OutDelayT")]
	private Label _OutDelayT;

	[AccessedThroughProperty("OutDelay")]
	private TrackBar _OutDelay;

	[AccessedThroughProperty("GroupBox3")]
	private GroupBox _GroupBox3;

	[AccessedThroughProperty("dspIDT")]
	private Label _dspIDT;

	[AccessedThroughProperty("GroupBox5")]
	private GroupBox _GroupBox5;

	[AccessedThroughProperty("GroupBox4")]
	private GroupBox _GroupBox4;

	[AccessedThroughProperty("Label12")]
	private Label _Label12;

	[AccessedThroughProperty("Label11")]
	private Label _Label11;

	[AccessedThroughProperty("Label10")]
	private Label _Label10;

	[AccessedThroughProperty("Label9")]
	private Label _Label9;

	[AccessedThroughProperty("Label7")]
	private Label _Label7;

	[AccessedThroughProperty("Label6")]
	private Label _Label6;

	[AccessedThroughProperty("Label4")]
	private Label _Label4;

	[AccessedThroughProperty("Label3")]
	private Label _Label3;

	[AccessedThroughProperty("Label2")]
	private Label _Label2;

	[AccessedThroughProperty("Label36")]
	private Label _Label36;

	[AccessedThroughProperty("Label25")]
	private Label _Label25;

	[AccessedThroughProperty("Label30")]
	private Label _Label30;

	[AccessedThroughProperty("Label35")]
	private Label _Label35;

	[AccessedThroughProperty("Label31")]
	private Label _Label31;

	[AccessedThroughProperty("Label24")]
	private Label _Label24;

	[AccessedThroughProperty("Label54")]
	private Label _Label54;

	[AccessedThroughProperty("Label50")]
	private Label _Label50;

	[AccessedThroughProperty("Label51")]
	private Label _Label51;

	[AccessedThroughProperty("Label48")]
	private Label _Label48;

	[AccessedThroughProperty("Label49")]
	private Label _Label49;

	[AccessedThroughProperty("Label37")]
	private Label _Label37;

	[AccessedThroughProperty("Label23")]
	private Label _Label23;

	[AccessedThroughProperty("Label22")]
	private Label _Label22;

	[AccessedThroughProperty("Label13")]
	private Label _Label13;

	[AccessedThroughProperty("Label14")]
	private Label _Label14;

	[AccessedThroughProperty("Label15")]
	private Label _Label15;

	[AccessedThroughProperty("MixVolTrabar")]
	private TrackBar _MixVolTrabar;

	[AccessedThroughProperty("Label55")]
	private Label _Label55;

	[AccessedThroughProperty("BT_IN4OUT4")]
	private Label _BT_IN4OUT4;

	[AccessedThroughProperty("BT_IN4OUT3")]
	private Label _BT_IN4OUT3;

	[AccessedThroughProperty("BT_IN4OUT2")]
	private Label _BT_IN4OUT2;

	[AccessedThroughProperty("BT_IN4OUT1")]
	private Label _BT_IN4OUT1;

	[AccessedThroughProperty("BT_IN2OUT4")]
	private Label _BT_IN2OUT4;

	[AccessedThroughProperty("BT_IN2OUT3")]
	private Label _BT_IN2OUT3;

	[AccessedThroughProperty("BT_IN2OUT2")]
	private Label _BT_IN2OUT2;

	[AccessedThroughProperty("BT_IN2OUT1")]
	private Label _BT_IN2OUT1;

	[AccessedThroughProperty("BT_IN3OUT4")]
	private Label _BT_IN3OUT4;

	[AccessedThroughProperty("BT_IN3OUT3")]
	private Label _BT_IN3OUT3;

	[AccessedThroughProperty("BT_IN3OUT2")]
	private Label _BT_IN3OUT2;

	[AccessedThroughProperty("BT_IN3OUT1")]
	private Label _BT_IN3OUT1;

	[AccessedThroughProperty("BT_IN1OUT4")]
	private Label _BT_IN1OUT4;

	[AccessedThroughProperty("BT_IN1OUT3")]
	private Label _BT_IN1OUT3;

	[AccessedThroughProperty("BT_IN1OUT2")]
	private Label _BT_IN1OUT2;

	[AccessedThroughProperty("BT_IN1OUT1")]
	private Label _BT_IN1OUT1;

	[AccessedThroughProperty("MixVolUpdown")]
	private DomainUpDown _MixVolUpdown;

	[AccessedThroughProperty("RouteClear")]
	private Button _RouteClear;

	[AccessedThroughProperty("RouteReset")]
	private Button _RouteReset;

	[AccessedThroughProperty("GainOut4")]
	private DomainUpDown _GainOut4;

	[AccessedThroughProperty("GainOut1")]
	private DomainUpDown _GainOut1;

	[AccessedThroughProperty("GainOut3")]
	private DomainUpDown _GainOut3;

	[AccessedThroughProperty("GainOut2")]
	private DomainUpDown _GainOut2;

	[AccessedThroughProperty("GainIn4")]
	private DomainUpDown _GainIn4;

	[AccessedThroughProperty("GainIn3")]
	private DomainUpDown _GainIn3;

	[AccessedThroughProperty("GainIn2")]
	private DomainUpDown _GainIn2;

	[AccessedThroughProperty("GainIn1")]
	private DomainUpDown _GainIn1;

	[AccessedThroughProperty("PhaseOut4")]
	private Button _PhaseOut4;

	[AccessedThroughProperty("PhaseOut3")]
	private Button _PhaseOut3;

	[AccessedThroughProperty("PhaseOut2")]
	private Button _PhaseOut2;

	[AccessedThroughProperty("PhaseOut1")]
	private Button _PhaseOut1;

	[AccessedThroughProperty("MuteOut4")]
	private Button _MuteOut4;

	[AccessedThroughProperty("MuteOut3")]
	private Button _MuteOut3;

	[AccessedThroughProperty("MuteOut2")]
	private Button _MuteOut2;

	[AccessedThroughProperty("MuteOut1")]
	private Button _MuteOut1;

	[AccessedThroughProperty("LimOut4")]
	private TextBox _LimOut4;

	[AccessedThroughProperty("LimOut3")]
	private TextBox _LimOut3;

	[AccessedThroughProperty("LimOut2")]
	private TextBox _LimOut2;

	[AccessedThroughProperty("LimOut1")]
	private TextBox _LimOut1;

	[AccessedThroughProperty("DelayOut4")]
	private TextBox _DelayOut4;

	[AccessedThroughProperty("DelayOut3")]
	private TextBox _DelayOut3;

	[AccessedThroughProperty("DelayOut2")]
	private TextBox _DelayOut2;

	[AccessedThroughProperty("DelayOut1")]
	private TextBox _DelayOut1;

	[AccessedThroughProperty("SelOut3")]
	private Button _SelOut3;

	[AccessedThroughProperty("SelOut4")]
	private Button _SelOut4;

	[AccessedThroughProperty("SelOut1")]
	private Button _SelOut1;

	[AccessedThroughProperty("SelOut2")]
	private Button _SelOut2;

	[AccessedThroughProperty("MarsterVolT")]
	private Label _MarsterVolT;

	[AccessedThroughProperty("MarsterVol")]
	private TrackBar _MarsterVol;

	[AccessedThroughProperty("Panel7")]
	private Panel _Panel7;

	[AccessedThroughProperty("Label1")]
	private Label _Label1;

	[AccessedThroughProperty("Label29")]
	private Label _Label29;

	[AccessedThroughProperty("Label59")]
	private Label _Label59;

	[AccessedThroughProperty("Label68")]
	private Label _Label68;

	[AccessedThroughProperty("FREQ_OUT27")]
	private DomainUpDown _FREQ_OUT27;

	[AccessedThroughProperty("GAIN_OUT27")]
	private DomainUpDown _GAIN_OUT27;

	[AccessedThroughProperty("Q_OUT27")]
	private DomainUpDown _Q_OUT27;

	[AccessedThroughProperty("GAIN_OUTTB27")]
	private TrackBar _GAIN_OUTTB27;

	[AccessedThroughProperty("Panel8")]
	private Panel _Panel8;

	[AccessedThroughProperty("Label69")]
	private Label _Label69;

	[AccessedThroughProperty("Label75")]
	private Label _Label75;

	[AccessedThroughProperty("Label76")]
	private Label _Label76;

	[AccessedThroughProperty("Label79")]
	private Label _Label79;

	[AccessedThroughProperty("FREQ_OUT26")]
	private DomainUpDown _FREQ_OUT26;

	[AccessedThroughProperty("GAIN_OUT26")]
	private DomainUpDown _GAIN_OUT26;

	[AccessedThroughProperty("Q_OUT26")]
	private DomainUpDown _Q_OUT26;

	[AccessedThroughProperty("GAIN_OUTTB26")]
	private TrackBar _GAIN_OUTTB26;

	[AccessedThroughProperty("GroupBox6")]
	private GroupBox _GroupBox6;

	[AccessedThroughProperty("Label271")]
	private Label _Label271;

	[AccessedThroughProperty("Label276")]
	private Label _Label276;

	[AccessedThroughProperty("TH_Noise")]
	private TrackBar _TH_Noise;

	[AccessedThroughProperty("Label519")]
	private Label _Label519;

	[AccessedThroughProperty("SuperTabControlPanel26")]
	private SuperTabControlPanel _SuperTabControlPanel26;

	[AccessedThroughProperty("InPic")]
	private PictureBox _InPic;

	[AccessedThroughProperty("BYPASS_IN")]
	private ButtonX _BYPASS_IN;

	[AccessedThroughProperty("RESET_IN")]
	private ButtonX _RESET_IN;

	[AccessedThroughProperty("Panel_in")]
	private Panel _Panel_in;

	[AccessedThroughProperty("Label138")]
	private Label _Label138;

	[AccessedThroughProperty("Label139")]
	private Label _Label139;

	[AccessedThroughProperty("Label140")]
	private Label _Label140;

	[AccessedThroughProperty("Label154")]
	private Label _Label154;

	[AccessedThroughProperty("in_f")]
	private DomainUpDown _in_f;

	[AccessedThroughProperty("in_g")]
	private DomainUpDown _in_g;

	[AccessedThroughProperty("in_q")]
	private DomainUpDown _in_q;

	[AccessedThroughProperty("in_bar")]
	private TrackBar _in_bar;

	[AccessedThroughProperty("Label200")]
	private Label _Label200;

	[AccessedThroughProperty("Label207")]
	private Label _Label207;

	[AccessedThroughProperty("Label259")]
	private Label _Label259;

	[AccessedThroughProperty("SuperTabItem1")]
	private SuperTabItem _SuperTabItem1;

	[AccessedThroughProperty("ShapeContainer1")]
	private ShapeContainer _ShapeContainer1;

	[AccessedThroughProperty("LINE_IN2OUT3")]
	private LineShape _LINE_IN2OUT3;

	[AccessedThroughProperty("LINE_IN3OUT1")]
	private LineShape _LINE_IN3OUT1;

	[AccessedThroughProperty("LINE_IN4OUT3")]
	private LineShape _LINE_IN4OUT3;

	[AccessedThroughProperty("LINE_IN2OUT1")]
	private LineShape _LINE_IN2OUT1;

	[AccessedThroughProperty("LINE_IN2OUT4")]
	private LineShape _LINE_IN2OUT4;

	[AccessedThroughProperty("LINE_IN4OUT2")]
	private LineShape _LINE_IN4OUT2;

	[AccessedThroughProperty("LINE_IN4OUT1")]
	private LineShape _LINE_IN4OUT1;

	[AccessedThroughProperty("LINE_IN3OUT4")]
	private LineShape _LINE_IN3OUT4;

	[AccessedThroughProperty("LINE_IN3OUT2")]
	private LineShape _LINE_IN3OUT2;

	[AccessedThroughProperty("LINE_IN1OUT4")]
	private LineShape _LINE_IN1OUT4;

	[AccessedThroughProperty("LINE_IN1OUT3")]
	private LineShape _LINE_IN1OUT3;

	[AccessedThroughProperty("LINE_IN1OUT2")]
	private LineShape _LINE_IN1OUT2;

	[AccessedThroughProperty("LINE_IN4OUT4")]
	private LineShape _LINE_IN4OUT4;

	[AccessedThroughProperty("LINE_IN3OUT3")]
	private LineShape _LINE_IN3OUT3;

	[AccessedThroughProperty("LINE_IN2OUT2")]
	private LineShape _LINE_IN2OUT2;

	[AccessedThroughProperty("LINE_IN1OUT1")]
	private LineShape _LINE_IN1OUT1;

	[AccessedThroughProperty("GroupBox7")]
	private GroupBox _GroupBox7;

	[AccessedThroughProperty("SelIn1")]
	private Button _SelIn1;

	[AccessedThroughProperty("Decay_NoiseT")]
	private Label _Decay_NoiseT;

	[AccessedThroughProperty("AT_NoiseT")]
	private Label _AT_NoiseT;

	[AccessedThroughProperty("TH_NoiseT")]
	private Label _TH_NoiseT;

	[AccessedThroughProperty("Decay_Noise")]
	private TrackBar _Decay_Noise;

	[AccessedThroughProperty("AT_Noise")]
	private TrackBar _AT_Noise;

	[AccessedThroughProperty("SelIn3")]
	private Button _SelIn3;

	[AccessedThroughProperty("SelIn4")]
	private Button _SelIn4;

	[AccessedThroughProperty("SelIn2")]
	private Button _SelIn2;

	[AccessedThroughProperty("Label80")]
	private Label _Label80;

	[AccessedThroughProperty("Panel_EQ")]
	private Panel _Panel_EQ;

	[AccessedThroughProperty("GroupBox8")]
	private GroupBox _GroupBox8;

	[AccessedThroughProperty("Label118")]
	private Label _Label118;

	[AccessedThroughProperty("Label117")]
	private Label _Label117;

	[AccessedThroughProperty("Label81")]
	private Label _Label81;

	[AccessedThroughProperty("FirHpfType")]
	private ComboBox _FirHpfType;

	[AccessedThroughProperty("Label38")]
	private Label _Label38;

	[AccessedThroughProperty("FirHpfT")]
	private Label _FirHpfT;

	[AccessedThroughProperty("FirLpfType")]
	private ComboBox _FirLpfType;

	[AccessedThroughProperty("FirHpf")]
	private TrackBar _FirHpf;

	[AccessedThroughProperty("FirLpfT")]
	private Label _FirLpfT;

	[AccessedThroughProperty("FirLpf")]
	private TrackBar _FirLpf;

	[AccessedThroughProperty("GroupBox32")]
	private GroupBox _GroupBox32;

	[AccessedThroughProperty("inGainT")]
	private Label _inGainT;

	[AccessedThroughProperty("Label120")]
	private Label _Label120;

	[AccessedThroughProperty("inGain")]
	private TrackBar _inGain;

	[AccessedThroughProperty("dspid")]
	private ComboBox _dspid;

	[AccessedThroughProperty("MarsterVoll")]
	private Label _MarsterVoll;

	[AccessedThroughProperty("ContextMenuStrip1")]
	private ContextMenuStrip _ContextMenuStrip1;

	[AccessedThroughProperty("复制输出曲线ToolStripMenuItem")]
	private ToolStripMenuItem _复制输出曲线ToolStripMenuItem;

	[AccessedThroughProperty("粘贴输入曲线ToolStripMenuItem")]
	private ToolStripMenuItem _粘贴输入曲线ToolStripMenuItem;

	[AccessedThroughProperty("ContextMenuStrip2")]
	private ContextMenuStrip _ContextMenuStrip2;

	[AccessedThroughProperty("copy")]
	private ToolStripMenuItem _copy;

	[AccessedThroughProperty("paster")]
	private ToolStripMenuItem _paster;

	[AccessedThroughProperty("SuperTabControlPanel27")]
	private SuperTabControlPanel _SuperTabControlPanel27;

	[AccessedThroughProperty("DevicePanel")]
	private Panel _DevicePanel;

	[AccessedThroughProperty("GroupBox36")]
	private GroupBox _GroupBox36;

	[AccessedThroughProperty("DeviceModeSel")]
	private ComboBox _DeviceModeSel;

	[AccessedThroughProperty("SaveMode1")]
	private Button _SaveMode1;

	[AccessedThroughProperty("RecallMode")]
	private Button _RecallMode;

	[AccessedThroughProperty("Label115")]
	private Label _Label115;

	[AccessedThroughProperty("GroupBox43")]
	private GroupBox _GroupBox43;

	[AccessedThroughProperty("clipT")]
	private Label _clipT;

	[AccessedThroughProperty("clip_OK")]
	private Button _clip_OK;

	[AccessedThroughProperty("clip")]
	private TrackBar _clip;

	[AccessedThroughProperty("GroupBox44")]
	private GroupBox _GroupBox44;

	[AccessedThroughProperty("v220")]
	private RadioButton _v220;

	[AccessedThroughProperty("v110")]
	private RadioButton _v110;

	[AccessedThroughProperty("v_ok")]
	private Button _v_ok;

	[AccessedThroughProperty("GroupBox45")]
	private GroupBox _GroupBox45;

	[AccessedThroughProperty("bootCharOK")]
	private Button _bootCharOK;

	[AccessedThroughProperty("bootCharT")]
	private TextBox _bootCharT;

	[AccessedThroughProperty("Label116")]
	private Label _Label116;

	[AccessedThroughProperty("GroupBox46")]
	private GroupBox _GroupBox46;

	[AccessedThroughProperty("AdminMode")]
	private RadioButton _AdminMode;

	[AccessedThroughProperty("UserMode")]
	private RadioButton _UserMode;

	[AccessedThroughProperty("UserAdminSure")]
	private Button _UserAdminSure;

	[AccessedThroughProperty("KeyChangeGrp")]
	private GroupBox _KeyChangeGrp;

	[AccessedThroughProperty("ModifyKeySure")]
	private Button _ModifyKeySure;

	[AccessedThroughProperty("NewKeySure")]
	private TextBox _NewKeySure;

	[AccessedThroughProperty("Label130")]
	private Label _Label130;

	[AccessedThroughProperty("NewKeyLb")]
	private Label _NewKeyLb;

	[AccessedThroughProperty("NewKeySureLb")]
	private Label _NewKeySureLb;

	[AccessedThroughProperty("OldKey")]
	private TextBox _OldKey;

	[AccessedThroughProperty("NewKey")]
	private TextBox _NewKey;

	[AccessedThroughProperty("PcPresetGrp")]
	private GroupBox _PcPresetGrp;

	[AccessedThroughProperty("UploadToDevice")]
	private Button _UploadToDevice;

	[AccessedThroughProperty("DelectMode")]
	private Button _DelectMode;

	[AccessedThroughProperty("ListView1")]
	private ListView _ListView1;

	[AccessedThroughProperty("ColumnHeader1")]
	private ColumnHeader _ColumnHeader1;

	[AccessedThroughProperty("ColumnHeader2")]
	private ColumnHeader _ColumnHeader2;

	[AccessedThroughProperty("PCModeName")]
	private TextBox _PCModeName;

	[AccessedThroughProperty("SaveMode")]
	private Button _SaveMode;

	[AccessedThroughProperty("Label137")]
	private Label _Label137;

	[AccessedThroughProperty("SaveModeToPC")]
	private Button _SaveModeToPC;

	[AccessedThroughProperty("SuperTabItem2")]
	private SuperTabItem _SuperTabItem2;

	[AccessedThroughProperty("Label198")]
	private Label _Label198;

	[AccessedThroughProperty("Label197")]
	private Label _Label197;

	[AccessedThroughProperty("Label160")]
	private Label _Label160;

	[AccessedThroughProperty("RadioButton16")]
	private RadioButton _RadioButton16;

	[AccessedThroughProperty("RadioButton17")]
	private RadioButton _RadioButton17;

	[AccessedThroughProperty("RadioButton18")]
	private RadioButton _RadioButton18;

	[AccessedThroughProperty("Label199")]
	private Label _Label199;

	[AccessedThroughProperty("Label201")]
	private Label _Label201;

	[AccessedThroughProperty("Label202")]
	private Label _Label202;

	[AccessedThroughProperty("RadioButton_L7")]
	private RadioButton _RadioButton_L7;

	[AccessedThroughProperty("RadioButton_H7")]
	private RadioButton _RadioButton_H7;

	[AccessedThroughProperty("RadioButton_P7")]
	private RadioButton _RadioButton_P7;

	[AccessedThroughProperty("RadioButton_L6")]
	private RadioButton _RadioButton_L6;

	[AccessedThroughProperty("RadioButton_H6")]
	private RadioButton _RadioButton_H6;

	[AccessedThroughProperty("RadioButton_P6")]
	private RadioButton _RadioButton_P6;

	[AccessedThroughProperty("RadioButton_L5")]
	private RadioButton _RadioButton_L5;

	[AccessedThroughProperty("RadioButton_H5")]
	private RadioButton _RadioButton_H5;

	[AccessedThroughProperty("RadioButton_P5")]
	private RadioButton _RadioButton_P5;

	[AccessedThroughProperty("RadioButton_L2")]
	private RadioButton _RadioButton_L2;

	[AccessedThroughProperty("RadioButton_H2")]
	private RadioButton _RadioButton_H2;

	[AccessedThroughProperty("RadioButton_P2")]
	private RadioButton _RadioButton_P2;

	[AccessedThroughProperty("RadioButton_L1")]
	private RadioButton _RadioButton_L1;

	[AccessedThroughProperty("RadioButton_H1")]
	private RadioButton _RadioButton_H1;

	[AccessedThroughProperty("RadioButton_P1")]
	private RadioButton _RadioButton_P1;

	[AccessedThroughProperty("RadioButton_L3")]
	private RadioButton _RadioButton_L3;

	[AccessedThroughProperty("RadioButton_H3")]
	private RadioButton _RadioButton_H3;

	[AccessedThroughProperty("RadioButton_P3")]
	private RadioButton _RadioButton_P3;

	[AccessedThroughProperty("RadioButton_L4")]
	private RadioButton _RadioButton_L4;

	[AccessedThroughProperty("RadioButton_H4")]
	private RadioButton _RadioButton_H4;

	[AccessedThroughProperty("RadioButton_P4")]
	private RadioButton _RadioButton_P4;

	[AccessedThroughProperty("Label204")]
	private Label _Label204;

	[AccessedThroughProperty("Label203")]
	private Label _Label203;

	[AccessedThroughProperty("WriteDSP")]
	private Button _WriteDSP;

	[AccessedThroughProperty("Flow_pannel")]
	private Panel _Flow_pannel;

	[AccessedThroughProperty("GroupBox47")]
	private GroupBox _GroupBox47;

	[AccessedThroughProperty("ListView2")]
	private ListView _ListView2;

	[AccessedThroughProperty("ColumnHeader3")]
	private ColumnHeader _ColumnHeader3;

	[AccessedThroughProperty("ColumnHeader4")]
	private ColumnHeader _ColumnHeader4;

	[AccessedThroughProperty("ColumnHeader5")]
	private ColumnHeader _ColumnHeader5;

	[AccessedThroughProperty("DeviceScan")]
	private Button _DeviceScan;

	[AccessedThroughProperty("ContextMenuDevice")]
	private ContextMenuStrip _ContextMenuDevice;

	[AccessedThroughProperty("DeviceToGroup")]
	private ToolStripMenuItem _DeviceToGroup;

	[AccessedThroughProperty("联机ToolStripMenuItem")]
	private ToolStripMenuItem _联机ToolStripMenuItem;

	[AccessedThroughProperty("Label267")]
	private Label _Label267;

	[AccessedThroughProperty("GroupBox48")]
	private GroupBox _GroupBox48;

	[AccessedThroughProperty("Label268")]
	private Label _Label268;

	[AccessedThroughProperty("Panel187")]
	private Panel _Panel187;

	[AccessedThroughProperty("level_out43")]
	private Panel _level_out43;

	[AccessedThroughProperty("level_out41")]
	private Panel _level_out41;

	[AccessedThroughProperty("Label750")]
	private Label _Label750;

	[AccessedThroughProperty("level_out42")]
	private Panel _level_out42;

	[AccessedThroughProperty("level_out44")]
	private Panel _level_out44;

	[AccessedThroughProperty("Label751")]
	private Label _Label751;

	[AccessedThroughProperty("Label752")]
	private Label _Label752;

	[AccessedThroughProperty("Label753")]
	private Label _Label753;

	[AccessedThroughProperty("level_out45")]
	private Panel _level_out45;

	[AccessedThroughProperty("Label754")]
	private Label _Label754;

	[AccessedThroughProperty("Label755")]
	private Label _Label755;

	[AccessedThroughProperty("level_out46")]
	private Panel _level_out46;

	[AccessedThroughProperty("Panel194")]
	private Panel _Panel194;

	[AccessedThroughProperty("level_out33")]
	private Panel _level_out33;

	[AccessedThroughProperty("level_out31")]
	private Panel _level_out31;

	[AccessedThroughProperty("Label756")]
	private Label _Label756;

	[AccessedThroughProperty("level_out32")]
	private Panel _level_out32;

	[AccessedThroughProperty("level_out34")]
	private Panel _level_out34;

	[AccessedThroughProperty("Label765")]
	private Label _Label765;

	[AccessedThroughProperty("Label766")]
	private Label _Label766;

	[AccessedThroughProperty("Label767")]
	private Label _Label767;

	[AccessedThroughProperty("level_out35")]
	private Panel _level_out35;

	[AccessedThroughProperty("Label768")]
	private Label _Label768;

	[AccessedThroughProperty("Label769")]
	private Label _Label769;

	[AccessedThroughProperty("level_out36")]
	private Panel _level_out36;

	[AccessedThroughProperty("Panel201")]
	private Panel _Panel201;

	[AccessedThroughProperty("level_out23")]
	private Panel _level_out23;

	[AccessedThroughProperty("level_out21")]
	private Panel _level_out21;

	[AccessedThroughProperty("Label770")]
	private Label _Label770;

	[AccessedThroughProperty("level_out22")]
	private Panel _level_out22;

	[AccessedThroughProperty("level_out24")]
	private Panel _level_out24;

	[AccessedThroughProperty("Label771")]
	private Label _Label771;

	[AccessedThroughProperty("Label772")]
	private Label _Label772;

	[AccessedThroughProperty("Label778")]
	private Label _Label778;

	[AccessedThroughProperty("level_out25")]
	private Panel _level_out25;

	[AccessedThroughProperty("Label781")]
	private Label _Label781;

	[AccessedThroughProperty("Label782")]
	private Label _Label782;

	[AccessedThroughProperty("level_out26")]
	private Panel _level_out26;

	[AccessedThroughProperty("Panel208")]
	private Panel _Panel208;

	[AccessedThroughProperty("level_out13")]
	private Panel _level_out13;

	[AccessedThroughProperty("level_out11")]
	private Panel _level_out11;

	[AccessedThroughProperty("Label783")]
	private Label _Label783;

	[AccessedThroughProperty("level_out12")]
	private Panel _level_out12;

	[AccessedThroughProperty("level_out14")]
	private Panel _level_out14;

	[AccessedThroughProperty("Label826")]
	private Label _Label826;

	[AccessedThroughProperty("Label827")]
	private Label _Label827;

	[AccessedThroughProperty("Label830")]
	private Label _Label830;

	[AccessedThroughProperty("level_out15")]
	private Panel _level_out15;

	[AccessedThroughProperty("Label832")]
	private Label _Label832;

	[AccessedThroughProperty("Label833")]
	private Label _Label833;

	[AccessedThroughProperty("level_out16")]
	private Panel _level_out16;

	[AccessedThroughProperty("Panel180")]
	private Panel _Panel180;

	[AccessedThroughProperty("level_in43")]
	private Panel _level_in43;

	[AccessedThroughProperty("level_in41")]
	private Panel _level_in41;

	[AccessedThroughProperty("Label604")]
	private Label _Label604;

	[AccessedThroughProperty("level_in42")]
	private Panel _level_in42;

	[AccessedThroughProperty("level_in44")]
	private Panel _level_in44;

	[AccessedThroughProperty("Label605")]
	private Label _Label605;

	[AccessedThroughProperty("Label606")]
	private Label _Label606;

	[AccessedThroughProperty("Label607")]
	private Label _Label607;

	[AccessedThroughProperty("level_in45")]
	private Panel _level_in45;

	[AccessedThroughProperty("Label738")]
	private Label _Label738;

	[AccessedThroughProperty("Label749")]
	private Label _Label749;

	[AccessedThroughProperty("level_in46")]
	private Panel _level_in46;

	[AccessedThroughProperty("Panel83")]
	private Panel _Panel83;

	[AccessedThroughProperty("level_in33")]
	private Panel _level_in33;

	[AccessedThroughProperty("level_in31")]
	private Panel _level_in31;

	[AccessedThroughProperty("Label598")]
	private Label _Label598;

	[AccessedThroughProperty("level_in32")]
	private Panel _level_in32;

	[AccessedThroughProperty("level_in34")]
	private Panel _level_in34;

	[AccessedThroughProperty("Label599")]
	private Label _Label599;

	[AccessedThroughProperty("Label600")]
	private Label _Label600;

	[AccessedThroughProperty("Label601")]
	private Label _Label601;

	[AccessedThroughProperty("level_in35")]
	private Panel _level_in35;

	[AccessedThroughProperty("Label602")]
	private Label _Label602;

	[AccessedThroughProperty("Label603")]
	private Label _Label603;

	[AccessedThroughProperty("level_in36")]
	private Panel _level_in36;

	[AccessedThroughProperty("Panel10")]
	private Panel _Panel10;

	[AccessedThroughProperty("level_in23")]
	private Panel _level_in23;

	[AccessedThroughProperty("level_in21")]
	private Panel _level_in21;

	[AccessedThroughProperty("Label592")]
	private Label _Label592;

	[AccessedThroughProperty("level_in22")]
	private Panel _level_in22;

	[AccessedThroughProperty("level_in24")]
	private Panel _level_in24;

	[AccessedThroughProperty("Label593")]
	private Label _Label593;

	[AccessedThroughProperty("Label594")]
	private Label _Label594;

	[AccessedThroughProperty("Label595")]
	private Label _Label595;

	[AccessedThroughProperty("level_in25")]
	private Panel _level_in25;

	[AccessedThroughProperty("Label596")]
	private Label _Label596;

	[AccessedThroughProperty("Label597")]
	private Label _Label597;

	[AccessedThroughProperty("level_in26")]
	private Panel _level_in26;

	[AccessedThroughProperty("Panel6")]
	private Panel _Panel6;

	[AccessedThroughProperty("level_in13")]
	private Panel _level_in13;

	[AccessedThroughProperty("level_in11")]
	private Panel _level_in11;

	[AccessedThroughProperty("Label588")]
	private Label _Label588;

	[AccessedThroughProperty("level_in12")]
	private Panel _level_in12;

	[AccessedThroughProperty("level_in14")]
	private Panel _level_in14;

	[AccessedThroughProperty("Label586")]
	private Label _Label586;

	[AccessedThroughProperty("Label589")]
	private Label _Label589;

	[AccessedThroughProperty("Label587")]
	private Label _Label587;

	[AccessedThroughProperty("level_in15")]
	private Panel _level_in15;

	[AccessedThroughProperty("Label591")]
	private Label _Label591;

	[AccessedThroughProperty("Label590")]
	private Label _Label590;

	[AccessedThroughProperty("level_in16")]
	private Panel _level_in16;

	[AccessedThroughProperty("Label585")]
	private Label _Label585;

	[AccessedThroughProperty("Label584")]
	private Label _Label584;

	[AccessedThroughProperty("Label583")]
	private Label _Label583;

	[AccessedThroughProperty("Label582")]
	private Label _Label582;

	[AccessedThroughProperty("SuperTabControlPanel22")]
	private SuperTabControlPanel _SuperTabControlPanel22;

	private const int NF = 420;

	private object lb;

	private object la;

	private double[] a;

	private double[] b;

	private const double PI = Math.PI;

	private const int Fs = 192000;

	private const int N = 960000;

	public byte[] RxBuf;

	public int RxCnt;

	private double[,] H;

	private Point[] XY;

	private Point[] CurXY;

	private Rectangle[] Rect;

	private bool[] EnableDrag;

	private bool[] QDrag;

	private Point[] OldPosition;

	private bool[] EQBypass;

	private byte EQno;

	private byte EQcnt;

	private byte PointCnt;

	private byte EQoffset;

	private byte[] EQnoTbl;

	private object[] EQoffsetTbl;

	private object[] EQcntTbl;

	private byte PageIndex;

	public byte StartPreset;

	private const int VendorID = 1155;

	private const int ProductID = 22352;

	private const int BufferInSize = 64;

	private const int BufferOutSize = 64;

	private byte[] BufferIn;

	private byte[] BufferOut;

	private float[] FreqTable;

	private string[] EQ_QTab;

	private string[] HLPF_QTab;

	private string[] HLpfTypeTab;

	private string[] LimiterRatio;

	private string[] LimiterAttackTime;

	private string[] LimiterDecayTime;

	private ArrayList FreqArray;

	private ArrayList GainArray;

	private ArrayList GainTrabarArray;

	private ArrayList QArray;

	private ArrayList TypeArray;

	private ArrayList TypePArray;

	private ArrayList TypeLSArray;

	private ArrayList TypeHSArray;

	private ArrayList LimiterThredholdArray;

	private ArrayList LimiterThredholdArrayT;

	private ArrayList LimiterAttTimeArray;

	private ArrayList LimiterDecTimeArray;

	private ArrayList LimiterRatioTimeArray;

	private ArrayList LimiterKneeTimeArray;

	private ArrayList NoiseGateThredholdArray;

	private ArrayList NoiseGateAttTimeArray;

	private ArrayList NoiseGateDecTimeArray;

	private ArrayList NoiseGateThredholdLableArray;

	private ArrayList NoiseGateAttTimeLableArray;

	private ArrayList NoiseGateDecTimeLableArray;

	private ArrayList LimiterThredholdLableArray;

	private ArrayList LimiterAttTimeLableArray;

	private ArrayList LimiterDecTimeLableArray;

	private ArrayList LimiterRatioTimeLableArray;

	private ArrayList LimiterKneeTimeLableArray;

	private ArrayList AmpTrabarArray;

	private ArrayList AmpTextboxArray;

	private ArrayList AmpPhaseArray;

	private ArrayList DelayArray;

	private ArrayList DelayLableArray;

	private ArrayList MixButtonArray;

	private ArrayList MixLineArray;

	private ArrayList InOutGainArrayUpdown;

	private ArrayList InOutGainArrayTrack;

	private ArrayList InOutGainArrayTrackT;

	private ArrayList HLpfFreqArray;

	private ArrayList HLpfLableArray;

	private ArrayList HLpfTypeArray;

	private ArrayList FirHLpfFreqArray;

	private ArrayList FirHLpfLableArray;

	private ArrayList FirHLpfTypeArray;

	private ArrayList PhaseArray;

	private ArrayList MuteArray;

	private ArrayList SelArray;

	private ArrayList PicArray;

	private ArrayList EQResetArray;

	private ArrayList EQBypassArray;

	public string SysName;

	private ArrayList LimPicArray;

	private ArrayList MusicSurce;

	private Panel[,] LevelDisArray;

	private bool ControlInited;

	private string DefaultPath;

	public int RX_LENGTH;

	private bool tabcontolOK;

	public byte WorkMode;

	public bool English;

	private Color[] styleColor;

	private Type_DSPID mDSPID;

	private int heartCnt;

	public object MusicEqData;

	private object PicMove;

	private int EQMoveCnt;

	private bool EQChanged;

	private bool InitEQ;

	private int[] SectionsTab;

	private double[] Qbuf;

	private const int FIR_TAPS = 1024;

	private double[] winBuf;

	private int CurQ;

	private bool MouseMove;

	private double[,,] HBackup;

	private bool FreqRespRestroe;

	private int PageIndex0Fst;

	private int PageIndex1Fst;

	private bool Tabinit;

	private bool FirNoDrawFreqResp;

	public bool Uart_OK;

	public bool ReceiveDis;

	public bool RxFinish;

	public byte UartRxState;

	private byte[] TmpBuf;

	private byte[] TuningCmdBuffer;

	private short rxstate;

	private short TuningCmdCnt;

	private short msglen;

	private bool UploadOK;

	private bool InitControlDataFlg;

	private bool DownloadOK;

	private bool UpdataOK;

	public bool SendEnable;

	public bool SendDelay;

	private System.Timers.Timer NoticeTimer1;

	private System.Timers.Timer TestTimer1;

	private int cnt;

	private bool Connected_OK;

	private bool[] BypassEQFlag;

	private byte BypassEQNo;

	private int selIndex;

	private bool EQ_HLpf_Limiter;

	private byte WorkModeSelect;

	private byte InCH;

	private byte OutCH;

	public bool AutoKeyOK;

	public bool AdminKeyOK;

	public string AdminKey;

	public string bootChar;

	private bool FirstUp;

	private bool TabControlManul;

	private System.Timers.Timer UartHeartBeatTimer;

	public byte UartHeartBeatTimer_Cnt;

	public byte RxFinish_Cnt;

	public int DisCnt;

	private bool UartHeartTest;

	public byte HeartBeatDisCnt;

	private bool connectAgain;

	private int sendNo;

	private int HeartBeatCnt;

	private int HeartBeatFlg;

	private int HeartBeatStart;

	private int disconCnt;

	private bool isHeartBeat;

	private int ProgramFileLength;

	private Rectangle[] LimRect;

	private bool[] LimEnableDrag;

	private Point[] LimOldPosition;

	private byte Limiteroffset;

	private double[] lim;

	private byte[] EQType;

	private bool EQTypeButton;

	public byte EchoRevEQNo;

	public byte EchoBRevBEQNo;

	private bool DelayReceive;

	private byte DelayReceiveCnt;

	private bool USB_OK;

	private int mHandle;

	public SYSREG SYSREGn;

	private byte BT_No;

	private bool SelOut;

	private byte inch_source;

	private byte outch_source;

	public bool RecallModeFlag;

	private Thread ReConnect;

	public bool PCRecall;

	public string HouZhuiMing;

	public string HouZhuiMing_dot;

	public byte HouZhuiMing_num;

	private byte n_dspid;

	private Point[,] flowPos;

	private int nDeviceGroup;

	private string CharStr;

	[SpecialName]
	private StaticLocalInitFlag _0024STATIC_0024Connect_Click_00242001_0024init_0024Init;

	[SpecialName]
	private StaticLocalInitFlag _0024STATIC_0024DeviceScan2_00242001_0024init_0024Init;

	[SpecialName]
	private object _0024STATIC_0024Connect_Click_00242001_0024init;

	[SpecialName]
	private object _0024STATIC_0024DeviceScan2_00242001_0024init;

	internal virtual PictureBox CenPic
	{
		get
		{
			return _CenPic;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			//IL_0008: Unknown result type (might be due to invalid IL or missing references)
			//IL_000e: Expected O, but got Unknown
			MouseEventHandler val = new MouseEventHandler(CenPic_MouseClick);
			if (_CenPic != null)
			{
				((Control)_CenPic).MouseClick -= val;
			}
			_CenPic = value;
			if (_CenPic != null)
			{
				((Control)_CenPic).MouseClick += val;
			}
		}
	}

	internal virtual SerialPort SerialPort1
	{
		get
		{
			return _SerialPort1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SerialPort1 = value;
		}
	}

	internal virtual OpenFileDialog OpenFileDialog1
	{
		get
		{
			return _OpenFileDialog1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_OpenFileDialog1 = value;
		}
	}

	internal virtual SaveFileDialog SaveFileDialog1
	{
		get
		{
			return _SaveFileDialog1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SaveFileDialog1 = value;
		}
	}

	internal virtual IntegerInput IntegerInput1
	{
		get
		{
			return _IntegerInput1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_IntegerInput1 = value;
		}
	}

	internal virtual SuperTabControlPanel SuperTabControlPanel1
	{
		get
		{
			return _SuperTabControlPanel1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SuperTabControlPanel1 = value;
		}
	}

	internal virtual SuperTabControlPanel SuperTabControlPanel2
	{
		get
		{
			return _SuperTabControlPanel2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SuperTabControlPanel2 = value;
		}
	}

	internal virtual SuperTabControl tabcontol
	{
		get
		{
			return _tabcontol;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			EventHandler<SuperTabStripSelectedTabChangedEventArgs> eventHandler = TabControl1_SelectedIndexChanged;
			if (_tabcontol != null)
			{
				_tabcontol.SelectedTabChanged -= eventHandler;
			}
			_tabcontol = value;
			if (_tabcontol != null)
			{
				_tabcontol.SelectedTabChanged += eventHandler;
			}
		}
	}

	internal virtual SuperTabControlPanel SuperTabControlPanel9
	{
		get
		{
			return _SuperTabControlPanel9;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SuperTabControlPanel9 = value;
		}
	}

	internal virtual SuperTabControlPanel SuperTabControlPanel8
	{
		get
		{
			return _SuperTabControlPanel8;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SuperTabControlPanel8 = value;
		}
	}

	internal virtual PictureBox SubPic
	{
		get
		{
			return _SubPic;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubPic = value;
		}
	}

	internal virtual ComboBox SubLpfSlope
	{
		get
		{
			return _SubLpfSlope;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubLpfSlope = value;
		}
	}

	internal virtual ComboBox SubHpfSlope
	{
		get
		{
			return _SubHpfSlope;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubHpfSlope = value;
		}
	}

	internal virtual Label Label500
	{
		get
		{
			return _Label500;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label500 = value;
		}
	}

	internal virtual Label Label442
	{
		get
		{
			return _Label442;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label442 = value;
		}
	}

	internal virtual Label Label512
	{
		get
		{
			return _Label512;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label512 = value;
		}
	}

	internal virtual Label Label452
	{
		get
		{
			return _Label452;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label452 = value;
		}
	}

	internal virtual Label Label463
	{
		get
		{
			return _Label463;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label463 = value;
		}
	}

	internal virtual Label SubRatioLable
	{
		get
		{
			return _SubRatioLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubRatioLable = value;
		}
	}

	internal virtual Label Label465
	{
		get
		{
			return _Label465;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label465 = value;
		}
	}

	internal virtual Label SubDecayTimeLable
	{
		get
		{
			return _SubDecayTimeLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubDecayTimeLable = value;
		}
	}

	internal virtual Label SubAttTimeLable
	{
		get
		{
			return _SubAttTimeLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubAttTimeLable = value;
		}
	}

	internal virtual Label Label469
	{
		get
		{
			return _Label469;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label469 = value;
		}
	}

	internal virtual Label SubMusicDelayLable
	{
		get
		{
			return _SubMusicDelayLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubMusicDelayLable = value;
		}
	}

	internal virtual Label SubLpfQLable
	{
		get
		{
			return _SubLpfQLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubLpfQLable = value;
		}
	}

	internal virtual Label SubHpfQLable
	{
		get
		{
			return _SubHpfQLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubHpfQLable = value;
		}
	}

	internal virtual Label SubLpfLable
	{
		get
		{
			return _SubLpfLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubLpfLable = value;
		}
	}

	internal virtual Label SubHpfLable
	{
		get
		{
			return _SubHpfLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubHpfLable = value;
		}
	}

	internal virtual Label SubThreholdLable
	{
		get
		{
			return _SubThreholdLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubThreholdLable = value;
		}
	}

	internal virtual Label SubRevVolPhase
	{
		get
		{
			return _SubRevVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubRevVolPhase = value;
		}
	}

	internal virtual Label SubRevVolLable
	{
		get
		{
			return _SubRevVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubRevVolLable = value;
		}
	}

	internal virtual Label SubEchoVolPhase
	{
		get
		{
			return _SubEchoVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubEchoVolPhase = value;
		}
	}

	internal virtual Label SubEchoVolLable
	{
		get
		{
			return _SubEchoVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubEchoVolLable = value;
		}
	}

	internal virtual Label SubMicVolPhase
	{
		get
		{
			return _SubMicVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubMicVolPhase = value;
		}
	}

	internal virtual Label SubMicVolLable
	{
		get
		{
			return _SubMicVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubMicVolLable = value;
		}
	}

	internal virtual Label SubMusicVolPhase
	{
		get
		{
			return _SubMusicVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubMusicVolPhase = value;
		}
	}

	internal virtual Label SubMusicVolLable
	{
		get
		{
			return _SubMusicVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubMusicVolLable = value;
		}
	}

	internal virtual Label Label481
	{
		get
		{
			return _Label481;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label481 = value;
		}
	}

	internal virtual Label Label482
	{
		get
		{
			return _Label482;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label482 = value;
		}
	}

	internal virtual Label Label483
	{
		get
		{
			return _Label483;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label483 = value;
		}
	}

	internal virtual Label Label484
	{
		get
		{
			return _Label484;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label484 = value;
		}
	}

	internal virtual Label Label488
	{
		get
		{
			return _Label488;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label488 = value;
		}
	}

	internal virtual Label Label489
	{
		get
		{
			return _Label489;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label489 = value;
		}
	}

	internal virtual Label Label490
	{
		get
		{
			return _Label490;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label490 = value;
		}
	}

	internal virtual Panel Panel63
	{
		get
		{
			return _Panel63;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel63 = value;
		}
	}

	internal virtual ButtonX SubHS2
	{
		get
		{
			return _SubHS2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubHS2 = value;
		}
	}

	internal virtual ButtonX SubLS2
	{
		get
		{
			return _SubLS2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubLS2 = value;
		}
	}

	internal virtual ButtonX SubP2
	{
		get
		{
			return _SubP2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubP2 = value;
		}
	}

	internal virtual Label Label492
	{
		get
		{
			return _Label492;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label492 = value;
		}
	}

	internal virtual Label Label493
	{
		get
		{
			return _Label493;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label493 = value;
		}
	}

	internal virtual Label Label494
	{
		get
		{
			return _Label494;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label494 = value;
		}
	}

	internal virtual DomainUpDown SubFreqEQ2
	{
		get
		{
			return _SubFreqEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubFreqEQ2 = value;
		}
	}

	internal virtual DomainUpDown SubGainEQ2
	{
		get
		{
			return _SubGainEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubGainEQ2 = value;
		}
	}

	internal virtual DomainUpDown SubQEQ2
	{
		get
		{
			return _SubQEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubQEQ2 = value;
		}
	}

	internal virtual TrackBar SubTrackBar2
	{
		get
		{
			return _SubTrackBar2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubTrackBar2 = value;
		}
	}

	internal virtual Label Label495
	{
		get
		{
			return _Label495;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label495 = value;
		}
	}

	internal virtual Label Label496
	{
		get
		{
			return _Label496;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label496 = value;
		}
	}

	internal virtual Panel Panel64
	{
		get
		{
			return _Panel64;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel64 = value;
		}
	}

	internal virtual ButtonX SubHS1
	{
		get
		{
			return _SubHS1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubHS1 = value;
		}
	}

	internal virtual ButtonX SubLS1
	{
		get
		{
			return _SubLS1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubLS1 = value;
		}
	}

	internal virtual ButtonX SubP1
	{
		get
		{
			return _SubP1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubP1 = value;
		}
	}

	internal virtual Label Label497
	{
		get
		{
			return _Label497;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label497 = value;
		}
	}

	internal virtual Label Label498
	{
		get
		{
			return _Label498;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label498 = value;
		}
	}

	internal virtual Label Label499
	{
		get
		{
			return _Label499;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label499 = value;
		}
	}

	internal virtual DomainUpDown SubFreqEQ1
	{
		get
		{
			return _SubFreqEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubFreqEQ1 = value;
		}
	}

	internal virtual DomainUpDown SubGainEQ1
	{
		get
		{
			return _SubGainEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubGainEQ1 = value;
		}
	}

	internal virtual DomainUpDown SubQEQ1
	{
		get
		{
			return _SubQEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubQEQ1 = value;
		}
	}

	internal virtual TrackBar SubTrackBar1
	{
		get
		{
			return _SubTrackBar1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubTrackBar1 = value;
		}
	}

	internal virtual Label Label487
	{
		get
		{
			return _Label487;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label487 = value;
		}
	}

	internal virtual Label SubLpfQText
	{
		get
		{
			return _SubLpfQText;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubLpfQText = value;
		}
	}

	internal virtual Label SubHpfQText
	{
		get
		{
			return _SubHpfQText;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubHpfQText = value;
		}
	}

	internal virtual Label Label451
	{
		get
		{
			return _Label451;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label451 = value;
		}
	}

	internal virtual Label Label502
	{
		get
		{
			return _Label502;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label502 = value;
		}
	}

	internal virtual Label Label503
	{
		get
		{
			return _Label503;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label503 = value;
		}
	}

	internal virtual Panel Panel65
	{
		get
		{
			return _Panel65;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel65 = value;
		}
	}

	internal virtual ButtonX SubHS3
	{
		get
		{
			return _SubHS3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubHS3 = value;
		}
	}

	internal virtual ButtonX SubLS3
	{
		get
		{
			return _SubLS3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubLS3 = value;
		}
	}

	internal virtual ButtonX SubP3
	{
		get
		{
			return _SubP3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubP3 = value;
		}
	}

	internal virtual Label Label504
	{
		get
		{
			return _Label504;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label504 = value;
		}
	}

	internal virtual Label Label505
	{
		get
		{
			return _Label505;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label505 = value;
		}
	}

	internal virtual Label Label506
	{
		get
		{
			return _Label506;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label506 = value;
		}
	}

	internal virtual DomainUpDown SubFreqEQ3
	{
		get
		{
			return _SubFreqEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubFreqEQ3 = value;
		}
	}

	internal virtual DomainUpDown SubGainEQ3
	{
		get
		{
			return _SubGainEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubGainEQ3 = value;
		}
	}

	internal virtual DomainUpDown SubQEQ3
	{
		get
		{
			return _SubQEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubQEQ3 = value;
		}
	}

	internal virtual TrackBar SubTrackBar3
	{
		get
		{
			return _SubTrackBar3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubTrackBar3 = value;
		}
	}

	internal virtual Label Label507
	{
		get
		{
			return _Label507;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label507 = value;
		}
	}

	internal virtual TrackBar SubRatio
	{
		get
		{
			return _SubRatio;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubRatio = value;
		}
	}

	internal virtual TrackBar SubRevVolTrabar
	{
		get
		{
			return _SubRevVolTrabar;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubRevVolTrabar = value;
		}
	}

	internal virtual TrackBar SubDecayTime
	{
		get
		{
			return _SubDecayTime;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubDecayTime = value;
		}
	}

	internal virtual TrackBar SubEchoVolTrabar
	{
		get
		{
			return _SubEchoVolTrabar;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubEchoVolTrabar = value;
		}
	}

	internal virtual TrackBar SubLpfQ
	{
		get
		{
			return _SubLpfQ;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			EventHandler eventHandler = SubLpfQ_Scroll;
			if (_SubLpfQ != null)
			{
				_SubLpfQ.ValueChanged -= eventHandler;
			}
			_SubLpfQ = value;
			if (_SubLpfQ != null)
			{
				_SubLpfQ.ValueChanged += eventHandler;
			}
		}
	}

	internal virtual TrackBar SubHpfQ
	{
		get
		{
			return _SubHpfQ;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			EventHandler eventHandler = SubHpfQ_Scroll;
			if (_SubHpfQ != null)
			{
				_SubHpfQ.ValueChanged -= eventHandler;
			}
			_SubHpfQ = value;
			if (_SubHpfQ != null)
			{
				_SubHpfQ.ValueChanged += eventHandler;
			}
		}
	}

	internal virtual TrackBar SubAttTime
	{
		get
		{
			return _SubAttTime;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubAttTime = value;
		}
	}

	internal virtual TrackBar SubMicVolTrabar
	{
		get
		{
			return _SubMicVolTrabar;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubMicVolTrabar = value;
		}
	}

	internal virtual TrackBar SubMusicDelay
	{
		get
		{
			return _SubMusicDelay;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubMusicDelay = value;
		}
	}

	internal virtual TrackBar SubLpf
	{
		get
		{
			return _SubLpf;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubLpf = value;
		}
	}

	internal virtual TrackBar SubHpf
	{
		get
		{
			return _SubHpf;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubHpf = value;
		}
	}

	internal virtual TrackBar SubThrehold
	{
		get
		{
			return _SubThrehold;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubThrehold = value;
		}
	}

	internal virtual TrackBar SubMusicVolTrabar
	{
		get
		{
			return _SubMusicVolTrabar;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubMusicVolTrabar = value;
		}
	}

	internal virtual SuperTabControlPanel SuperTabControlPanel7
	{
		get
		{
			return _SuperTabControlPanel7;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SuperTabControlPanel7 = value;
		}
	}

	internal virtual ComboBox CenHpfSlope
	{
		get
		{
			return _CenHpfSlope;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenHpfSlope = value;
		}
	}

	internal virtual Label Label417
	{
		get
		{
			return _Label417;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label417 = value;
		}
	}

	internal virtual Label Label411
	{
		get
		{
			return _Label411;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label411 = value;
		}
	}

	internal virtual Label Label412
	{
		get
		{
			return _Label412;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label412 = value;
		}
	}

	internal virtual Label CenRatioLable
	{
		get
		{
			return _CenRatioLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenRatioLable = value;
		}
	}

	internal virtual Label Label414
	{
		get
		{
			return _Label414;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label414 = value;
		}
	}

	internal virtual Label CenDecayTimeLable
	{
		get
		{
			return _CenDecayTimeLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenDecayTimeLable = value;
		}
	}

	internal virtual Label CenAttTimeLable
	{
		get
		{
			return _CenAttTimeLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenAttTimeLable = value;
		}
	}

	internal virtual Label Label419
	{
		get
		{
			return _Label419;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label419 = value;
		}
	}

	internal virtual Label CenMusicDelayLable
	{
		get
		{
			return _CenMusicDelayLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenMusicDelayLable = value;
		}
	}

	internal virtual Label CenHpfLable
	{
		get
		{
			return _CenHpfLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenHpfLable = value;
		}
	}

	internal virtual Label CenThreholdLable
	{
		get
		{
			return _CenThreholdLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenThreholdLable = value;
		}
	}

	internal virtual Label CenRevVolPhase
	{
		get
		{
			return _CenRevVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenRevVolPhase = value;
		}
	}

	internal virtual Label CenRevVolLable
	{
		get
		{
			return _CenRevVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenRevVolLable = value;
		}
	}

	internal virtual Label CenEchoVolPhase
	{
		get
		{
			return _CenEchoVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenEchoVolPhase = value;
		}
	}

	internal virtual Label CenEchoVolLable
	{
		get
		{
			return _CenEchoVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenEchoVolLable = value;
		}
	}

	internal virtual Label CenMicVolPhase
	{
		get
		{
			return _CenMicVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenMicVolPhase = value;
		}
	}

	internal virtual Label CenMicVolLable
	{
		get
		{
			return _CenMicVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenMicVolLable = value;
		}
	}

	internal virtual Label CenMusicVolPhase
	{
		get
		{
			return _CenMusicVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenMusicVolPhase = value;
		}
	}

	internal virtual Label CenMusicVolLable
	{
		get
		{
			return _CenMusicVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenMusicVolLable = value;
		}
	}

	internal virtual Label Label431
	{
		get
		{
			return _Label431;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label431 = value;
		}
	}

	internal virtual Label Label432
	{
		get
		{
			return _Label432;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label432 = value;
		}
	}

	internal virtual Label Label433
	{
		get
		{
			return _Label433;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label433 = value;
		}
	}

	internal virtual Label Label434
	{
		get
		{
			return _Label434;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label434 = value;
		}
	}

	internal virtual Panel Panel57
	{
		get
		{
			return _Panel57;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel57 = value;
		}
	}

	internal virtual ButtonX CenHS5
	{
		get
		{
			return _CenHS5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenHS5 = value;
		}
	}

	internal virtual ButtonX CenLS5
	{
		get
		{
			return _CenLS5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenLS5 = value;
		}
	}

	internal virtual ButtonX CenP5
	{
		get
		{
			return _CenP5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenP5 = value;
		}
	}

	internal virtual Label Label435
	{
		get
		{
			return _Label435;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label435 = value;
		}
	}

	internal virtual Label Label436
	{
		get
		{
			return _Label436;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label436 = value;
		}
	}

	internal virtual Label Label437
	{
		get
		{
			return _Label437;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label437 = value;
		}
	}

	internal virtual DomainUpDown CenFreqEQ5
	{
		get
		{
			return _CenFreqEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenFreqEQ5 = value;
		}
	}

	internal virtual DomainUpDown CenGainEQ5
	{
		get
		{
			return _CenGainEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenGainEQ5 = value;
		}
	}

	internal virtual DomainUpDown CenQEQ5
	{
		get
		{
			return _CenQEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenQEQ5 = value;
		}
	}

	internal virtual TrackBar CenTrackBar5
	{
		get
		{
			return _CenTrackBar5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenTrackBar5 = value;
		}
	}

	internal virtual Label Label438
	{
		get
		{
			return _Label438;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label438 = value;
		}
	}

	internal virtual Label Label439
	{
		get
		{
			return _Label439;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label439 = value;
		}
	}

	internal virtual Label Label440
	{
		get
		{
			return _Label440;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label440 = value;
		}
	}

	internal virtual Panel Panel58
	{
		get
		{
			return _Panel58;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel58 = value;
		}
	}

	internal virtual ButtonX CenHS2
	{
		get
		{
			return _CenHS2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenHS2 = value;
		}
	}

	internal virtual ButtonX CenLS2
	{
		get
		{
			return _CenLS2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenLS2 = value;
		}
	}

	internal virtual ButtonX CenP2
	{
		get
		{
			return _CenP2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenP2 = value;
		}
	}

	internal virtual Label Label443
	{
		get
		{
			return _Label443;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label443 = value;
		}
	}

	internal virtual Label Label444
	{
		get
		{
			return _Label444;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label444 = value;
		}
	}

	internal virtual Label Label445
	{
		get
		{
			return _Label445;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label445 = value;
		}
	}

	internal virtual DomainUpDown CenFreqEQ2
	{
		get
		{
			return _CenFreqEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenFreqEQ2 = value;
		}
	}

	internal virtual DomainUpDown CenGainEQ2
	{
		get
		{
			return _CenGainEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenGainEQ2 = value;
		}
	}

	internal virtual DomainUpDown CenQEQ2
	{
		get
		{
			return _CenQEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenQEQ2 = value;
		}
	}

	internal virtual TrackBar CenTrackBar2
	{
		get
		{
			return _CenTrackBar2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenTrackBar2 = value;
		}
	}

	internal virtual Label Label446
	{
		get
		{
			return _Label446;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label446 = value;
		}
	}

	internal virtual Label Label447
	{
		get
		{
			return _Label447;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label447 = value;
		}
	}

	internal virtual Panel Panel59
	{
		get
		{
			return _Panel59;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel59 = value;
		}
	}

	internal virtual ButtonX CenHS1
	{
		get
		{
			return _CenHS1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenHS1 = value;
		}
	}

	internal virtual ButtonX CenLS1
	{
		get
		{
			return _CenLS1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenLS1 = value;
		}
	}

	internal virtual ButtonX CenP1
	{
		get
		{
			return _CenP1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenP1 = value;
		}
	}

	internal virtual Label Label448
	{
		get
		{
			return _Label448;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label448 = value;
		}
	}

	internal virtual Label Label449
	{
		get
		{
			return _Label449;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label449 = value;
		}
	}

	internal virtual Label Label450
	{
		get
		{
			return _Label450;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label450 = value;
		}
	}

	internal virtual DomainUpDown CenFreqEQ1
	{
		get
		{
			return _CenFreqEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenFreqEQ1 = value;
		}
	}

	internal virtual DomainUpDown CenGainEQ1
	{
		get
		{
			return _CenGainEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenGainEQ1 = value;
		}
	}

	internal virtual DomainUpDown CenQEQ1
	{
		get
		{
			return _CenQEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenQEQ1 = value;
		}
	}

	internal virtual TrackBar CenTrackBar1
	{
		get
		{
			return _CenTrackBar1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenTrackBar1 = value;
		}
	}

	internal virtual Label Label453
	{
		get
		{
			return _Label453;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label453 = value;
		}
	}

	internal virtual Label Label454
	{
		get
		{
			return _Label454;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label454 = value;
		}
	}

	internal virtual Label Label455
	{
		get
		{
			return _Label455;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label455 = value;
		}
	}

	internal virtual Panel Panel60
	{
		get
		{
			return _Panel60;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel60 = value;
		}
	}

	internal virtual ButtonX CenHS3
	{
		get
		{
			return _CenHS3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenHS3 = value;
		}
	}

	internal virtual ButtonX CenLS3
	{
		get
		{
			return _CenLS3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenLS3 = value;
		}
	}

	internal virtual ButtonX CenP3
	{
		get
		{
			return _CenP3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenP3 = value;
		}
	}

	internal virtual Label Label456
	{
		get
		{
			return _Label456;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label456 = value;
		}
	}

	internal virtual Label Label457
	{
		get
		{
			return _Label457;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label457 = value;
		}
	}

	internal virtual Label Label458
	{
		get
		{
			return _Label458;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label458 = value;
		}
	}

	internal virtual DomainUpDown CenFreqEQ3
	{
		get
		{
			return _CenFreqEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenFreqEQ3 = value;
		}
	}

	internal virtual DomainUpDown CenGainEQ3
	{
		get
		{
			return _CenGainEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenGainEQ3 = value;
		}
	}

	internal virtual DomainUpDown CenQEQ3
	{
		get
		{
			return _CenQEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenQEQ3 = value;
		}
	}

	internal virtual TrackBar CenTrackBar3
	{
		get
		{
			return _CenTrackBar3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenTrackBar3 = value;
		}
	}

	internal virtual Label Label459
	{
		get
		{
			return _Label459;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label459 = value;
		}
	}

	internal virtual Panel Panel61
	{
		get
		{
			return _Panel61;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel61 = value;
		}
	}

	internal virtual ButtonX CenHS4
	{
		get
		{
			return _CenHS4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenHS4 = value;
		}
	}

	internal virtual ButtonX CenLS4
	{
		get
		{
			return _CenLS4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenLS4 = value;
		}
	}

	internal virtual ButtonX CenP4
	{
		get
		{
			return _CenP4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenP4 = value;
		}
	}

	internal virtual Label Label460
	{
		get
		{
			return _Label460;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label460 = value;
		}
	}

	internal virtual Label Label461
	{
		get
		{
			return _Label461;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label461 = value;
		}
	}

	internal virtual Label Label462
	{
		get
		{
			return _Label462;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label462 = value;
		}
	}

	internal virtual DomainUpDown CenFreqEQ4
	{
		get
		{
			return _CenFreqEQ4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenFreqEQ4 = value;
		}
	}

	internal virtual DomainUpDown CenGainEQ4
	{
		get
		{
			return _CenGainEQ4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenGainEQ4 = value;
		}
	}

	internal virtual DomainUpDown CenQEQ4
	{
		get
		{
			return _CenQEQ4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenQEQ4 = value;
		}
	}

	internal virtual TrackBar CenTrackBar4
	{
		get
		{
			return _CenTrackBar4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenTrackBar4 = value;
		}
	}

	internal virtual TrackBar CenRatio
	{
		get
		{
			return _CenRatio;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenRatio = value;
		}
	}

	internal virtual TrackBar CenRevVolTrabar
	{
		get
		{
			return _CenRevVolTrabar;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenRevVolTrabar = value;
		}
	}

	internal virtual TrackBar CenDecayTime
	{
		get
		{
			return _CenDecayTime;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenDecayTime = value;
		}
	}

	internal virtual TrackBar CenEchoVolTrabar
	{
		get
		{
			return _CenEchoVolTrabar;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenEchoVolTrabar = value;
		}
	}

	internal virtual TrackBar CenAttTime
	{
		get
		{
			return _CenAttTime;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenAttTime = value;
		}
	}

	internal virtual TrackBar CenMicVolTrabar
	{
		get
		{
			return _CenMicVolTrabar;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenMicVolTrabar = value;
		}
	}

	internal virtual TrackBar CenHpf
	{
		get
		{
			return _CenHpf;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenHpf = value;
		}
	}

	internal virtual TrackBar CenThrehold
	{
		get
		{
			return _CenThrehold;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenThrehold = value;
		}
	}

	internal virtual TrackBar CenMusicVolTrabar
	{
		get
		{
			return _CenMusicVolTrabar;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenMusicVolTrabar = value;
		}
	}

	internal virtual SuperTabControlPanel SuperTabControlPanel6
	{
		get
		{
			return _SuperTabControlPanel6;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SuperTabControlPanel6 = value;
		}
	}

	internal virtual PictureBox MainPic
	{
		get
		{
			return _MainPic;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainPic = value;
		}
	}

	internal virtual Label Label407
	{
		get
		{
			return _Label407;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label407 = value;
		}
	}

	internal virtual Label Label395
	{
		get
		{
			return _Label395;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label395 = value;
		}
	}

	internal virtual Label MainRatioLable
	{
		get
		{
			return _MainRatioLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainRatioLable = value;
		}
	}

	internal virtual Label Label392
	{
		get
		{
			return _Label392;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label392 = value;
		}
	}

	internal virtual Label MainDecayTImeLable
	{
		get
		{
			return _MainDecayTImeLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainDecayTImeLable = value;
		}
	}

	internal virtual Label MainAttTimeLable
	{
		get
		{
			return _MainAttTimeLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainAttTimeLable = value;
		}
	}

	internal virtual Label Label359
	{
		get
		{
			return _Label359;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label359 = value;
		}
	}

	internal virtual Label MainMusicDelayRLable
	{
		get
		{
			return _MainMusicDelayRLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainMusicDelayRLable = value;
		}
	}

	internal virtual Label MainMusicDelayLLable
	{
		get
		{
			return _MainMusicDelayLLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainMusicDelayLLable = value;
		}
	}

	internal virtual Label MainThreholdLable
	{
		get
		{
			return _MainThreholdLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainThreholdLable = value;
		}
	}

	internal virtual Label MainRevVolPhase
	{
		get
		{
			return _MainRevVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainRevVolPhase = value;
		}
	}

	internal virtual Label MainRevVolLable
	{
		get
		{
			return _MainRevVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainRevVolLable = value;
		}
	}

	internal virtual Label MainEchoVolPhase
	{
		get
		{
			return _MainEchoVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainEchoVolPhase = value;
		}
	}

	internal virtual Label MainEchoVolLable
	{
		get
		{
			return _MainEchoVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainEchoVolLable = value;
		}
	}

	internal virtual Label MainMicVolPhase
	{
		get
		{
			return _MainMicVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainMicVolPhase = value;
		}
	}

	internal virtual Label MainMicVolLable
	{
		get
		{
			return _MainMicVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainMicVolLable = value;
		}
	}

	internal virtual Label MainMusicVolPhase
	{
		get
		{
			return _MainMusicVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainMusicVolPhase = value;
		}
	}

	internal virtual Label MainMusicVolLable
	{
		get
		{
			return _MainMusicVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainMusicVolLable = value;
		}
	}

	internal virtual Label Label360
	{
		get
		{
			return _Label360;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label360 = value;
		}
	}

	internal virtual Label Label387
	{
		get
		{
			return _Label387;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label387 = value;
		}
	}

	internal virtual Label Label381
	{
		get
		{
			return _Label381;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label381 = value;
		}
	}

	internal virtual Label Label361
	{
		get
		{
			return _Label361;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label361 = value;
		}
	}

	internal virtual Panel Panel52
	{
		get
		{
			return _Panel52;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel52 = value;
		}
	}

	internal virtual ButtonX MainHS5
	{
		get
		{
			return _MainHS5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainHS5 = value;
		}
	}

	internal virtual ButtonX MainLS5
	{
		get
		{
			return _MainLS5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainLS5 = value;
		}
	}

	internal virtual ButtonX MainP5
	{
		get
		{
			return _MainP5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainP5 = value;
		}
	}

	internal virtual Label Label362
	{
		get
		{
			return _Label362;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label362 = value;
		}
	}

	internal virtual Label Label363
	{
		get
		{
			return _Label363;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label363 = value;
		}
	}

	internal virtual Label Label364
	{
		get
		{
			return _Label364;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label364 = value;
		}
	}

	internal virtual DomainUpDown MainFreqEQ5
	{
		get
		{
			return _MainFreqEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainFreqEQ5 = value;
		}
	}

	internal virtual DomainUpDown MainGainEQ5
	{
		get
		{
			return _MainGainEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainGainEQ5 = value;
		}
	}

	internal virtual DomainUpDown MainQEQ5
	{
		get
		{
			return _MainQEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainQEQ5 = value;
		}
	}

	internal virtual TrackBar MainTrackBar5
	{
		get
		{
			return _MainTrackBar5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainTrackBar5 = value;
		}
	}

	internal virtual Label Label386
	{
		get
		{
			return _Label386;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label386 = value;
		}
	}

	internal virtual Label Label380
	{
		get
		{
			return _Label380;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label380 = value;
		}
	}

	internal virtual Label Label365
	{
		get
		{
			return _Label365;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label365 = value;
		}
	}

	internal virtual Panel Panel53
	{
		get
		{
			return _Panel53;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel53 = value;
		}
	}

	internal virtual ButtonX MainHS2
	{
		get
		{
			return _MainHS2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainHS2 = value;
		}
	}

	internal virtual ButtonX MainLS2
	{
		get
		{
			return _MainLS2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainLS2 = value;
		}
	}

	internal virtual ButtonX MainP2
	{
		get
		{
			return _MainP2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainP2 = value;
		}
	}

	internal virtual Label Label366
	{
		get
		{
			return _Label366;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label366 = value;
		}
	}

	internal virtual Label Label367
	{
		get
		{
			return _Label367;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label367 = value;
		}
	}

	internal virtual Label Label368
	{
		get
		{
			return _Label368;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label368 = value;
		}
	}

	internal virtual DomainUpDown MainFreqEQ2
	{
		get
		{
			return _MainFreqEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainFreqEQ2 = value;
		}
	}

	internal virtual DomainUpDown MainGainEQ2
	{
		get
		{
			return _MainGainEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainGainEQ2 = value;
		}
	}

	internal virtual DomainUpDown MainQEQ2
	{
		get
		{
			return _MainQEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainQEQ2 = value;
		}
	}

	internal virtual TrackBar MainTrackBar2
	{
		get
		{
			return _MainTrackBar2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainTrackBar2 = value;
		}
	}

	internal virtual Label Label385
	{
		get
		{
			return _Label385;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label385 = value;
		}
	}

	internal virtual Label Label379
	{
		get
		{
			return _Label379;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label379 = value;
		}
	}

	internal virtual Panel Panel54
	{
		get
		{
			return _Panel54;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel54 = value;
		}
	}

	internal virtual ButtonX MainHS1
	{
		get
		{
			return _MainHS1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainHS1 = value;
		}
	}

	internal virtual ButtonX MainLS1
	{
		get
		{
			return _MainLS1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainLS1 = value;
		}
	}

	internal virtual ButtonX MainP1
	{
		get
		{
			return _MainP1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainP1 = value;
		}
	}

	internal virtual Label Label369
	{
		get
		{
			return _Label369;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label369 = value;
		}
	}

	internal virtual Label Label370
	{
		get
		{
			return _Label370;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label370 = value;
		}
	}

	internal virtual Label Label371
	{
		get
		{
			return _Label371;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label371 = value;
		}
	}

	internal virtual DomainUpDown MainFreqEQ1
	{
		get
		{
			return _MainFreqEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainFreqEQ1 = value;
		}
	}

	internal virtual DomainUpDown MainGainEQ1
	{
		get
		{
			return _MainGainEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainGainEQ1 = value;
		}
	}

	internal virtual DomainUpDown MainQEQ1
	{
		get
		{
			return _MainQEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainQEQ1 = value;
		}
	}

	internal virtual TrackBar MainTrackBar1
	{
		get
		{
			return _MainTrackBar1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainTrackBar1 = value;
		}
	}

	internal virtual Label Label406
	{
		get
		{
			return _Label406;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label406 = value;
		}
	}

	internal virtual Label Label398
	{
		get
		{
			return _Label398;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label398 = value;
		}
	}

	internal virtual Label Label399
	{
		get
		{
			return _Label399;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label399 = value;
		}
	}

	internal virtual Label Label396
	{
		get
		{
			return _Label396;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label396 = value;
		}
	}

	internal virtual Label Label384
	{
		get
		{
			return _Label384;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label384 = value;
		}
	}

	internal virtual Panel Panel55
	{
		get
		{
			return _Panel55;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel55 = value;
		}
	}

	internal virtual ButtonX MainHS3
	{
		get
		{
			return _MainHS3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainHS3 = value;
		}
	}

	internal virtual ButtonX MainLS3
	{
		get
		{
			return _MainLS3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainLS3 = value;
		}
	}

	internal virtual ButtonX MainP3
	{
		get
		{
			return _MainP3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainP3 = value;
		}
	}

	internal virtual Label Label372
	{
		get
		{
			return _Label372;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label372 = value;
		}
	}

	internal virtual Label Label373
	{
		get
		{
			return _Label373;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label373 = value;
		}
	}

	internal virtual Label Label374
	{
		get
		{
			return _Label374;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label374 = value;
		}
	}

	internal virtual DomainUpDown MainFreqEQ3
	{
		get
		{
			return _MainFreqEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainFreqEQ3 = value;
		}
	}

	internal virtual DomainUpDown MainGainEQ3
	{
		get
		{
			return _MainGainEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainGainEQ3 = value;
		}
	}

	internal virtual DomainUpDown MainQEQ3
	{
		get
		{
			return _MainQEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainQEQ3 = value;
		}
	}

	internal virtual TrackBar MainTrackBar3
	{
		get
		{
			return _MainTrackBar3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainTrackBar3 = value;
		}
	}

	internal virtual Label Label378
	{
		get
		{
			return _Label378;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label378 = value;
		}
	}

	internal virtual Panel Panel56
	{
		get
		{
			return _Panel56;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel56 = value;
		}
	}

	internal virtual ButtonX MainHS4
	{
		get
		{
			return _MainHS4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainHS4 = value;
		}
	}

	internal virtual ButtonX MainLS4
	{
		get
		{
			return _MainLS4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainLS4 = value;
		}
	}

	internal virtual ButtonX MainP4
	{
		get
		{
			return _MainP4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainP4 = value;
		}
	}

	internal virtual Label Label375
	{
		get
		{
			return _Label375;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label375 = value;
		}
	}

	internal virtual Label Label376
	{
		get
		{
			return _Label376;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label376 = value;
		}
	}

	internal virtual Label Label377
	{
		get
		{
			return _Label377;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label377 = value;
		}
	}

	internal virtual DomainUpDown MainFreqEQ4
	{
		get
		{
			return _MainFreqEQ4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainFreqEQ4 = value;
		}
	}

	internal virtual DomainUpDown MainGainEQ4
	{
		get
		{
			return _MainGainEQ4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainGainEQ4 = value;
		}
	}

	internal virtual DomainUpDown MainQEQ4
	{
		get
		{
			return _MainQEQ4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainQEQ4 = value;
		}
	}

	internal virtual TrackBar MainTrackBar4
	{
		get
		{
			return _MainTrackBar4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainTrackBar4 = value;
		}
	}

	internal virtual TrackBar MainRatio
	{
		get
		{
			return _MainRatio;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainRatio = value;
		}
	}

	internal virtual TrackBar MainDecayTIme
	{
		get
		{
			return _MainDecayTIme;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainDecayTIme = value;
		}
	}

	internal virtual TrackBar MainRevVolTrabar
	{
		get
		{
			return _MainRevVolTrabar;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainRevVolTrabar = value;
		}
	}

	internal virtual TrackBar MainAttTime
	{
		get
		{
			return _MainAttTime;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainAttTime = value;
		}
	}

	internal virtual TrackBar MainEchoVolTrabar
	{
		get
		{
			return _MainEchoVolTrabar;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainEchoVolTrabar = value;
		}
	}

	internal virtual TrackBar MainMusicDelayR
	{
		get
		{
			return _MainMusicDelayR;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainMusicDelayR = value;
		}
	}

	internal virtual TrackBar MainMusicDelayL
	{
		get
		{
			return _MainMusicDelayL;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainMusicDelayL = value;
		}
	}

	internal virtual TrackBar MainThrehold
	{
		get
		{
			return _MainThrehold;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainThrehold = value;
		}
	}

	internal virtual TrackBar MainMicVolTrabar
	{
		get
		{
			return _MainMicVolTrabar;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainMicVolTrabar = value;
		}
	}

	internal virtual TrackBar MainMusicVolTrabar
	{
		get
		{
			return _MainMusicVolTrabar;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainMusicVolTrabar = value;
		}
	}

	internal virtual SuperTabControlPanel SuperTabControlPanel10
	{
		get
		{
			return _SuperTabControlPanel10;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SuperTabControlPanel10 = value;
		}
	}

	internal virtual PictureBox RevPic
	{
		get
		{
			return _RevPic;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevPic = value;
		}
	}

	internal virtual Label Label645
	{
		get
		{
			return _Label645;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label645 = value;
		}
	}

	internal virtual Label RevTimeLable
	{
		get
		{
			return _RevTimeLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevTimeLable = value;
		}
	}

	internal virtual Label RevPreDelayLable
	{
		get
		{
			return _RevPreDelayLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevPreDelayLable = value;
		}
	}

	internal virtual Label Label652
	{
		get
		{
			return _Label652;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label652 = value;
		}
	}

	internal virtual Label Label653
	{
		get
		{
			return _Label653;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label653 = value;
		}
	}

	internal virtual TrackBar RevTime
	{
		get
		{
			return _RevTime;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevTime = value;
		}
	}

	internal virtual TrackBar RevPreDelay
	{
		get
		{
			return _RevPreDelay;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevPreDelay = value;
		}
	}

	internal virtual Label RevLpfLable
	{
		get
		{
			return _RevLpfLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevLpfLable = value;
		}
	}

	internal virtual Label RevHpfLable
	{
		get
		{
			return _RevHpfLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevHpfLable = value;
		}
	}

	internal virtual Label Label656
	{
		get
		{
			return _Label656;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label656 = value;
		}
	}

	internal virtual TrackBar RevLpf
	{
		get
		{
			return _RevLpf;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevLpf = value;
		}
	}

	internal virtual Label Label657
	{
		get
		{
			return _Label657;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label657 = value;
		}
	}

	internal virtual TrackBar RevHpf
	{
		get
		{
			return _RevHpf;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevHpf = value;
		}
	}

	internal virtual Label RevDirectVolPhase
	{
		get
		{
			return _RevDirectVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevDirectVolPhase = value;
		}
	}

	internal virtual Label RevDirectVolLable
	{
		get
		{
			return _RevDirectVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevDirectVolLable = value;
		}
	}

	internal virtual Label RevVolPhase
	{
		get
		{
			return _RevVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevVolPhase = value;
		}
	}

	internal virtual Label RevVolLable
	{
		get
		{
			return _RevVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevVolLable = value;
		}
	}

	internal virtual Label Label662
	{
		get
		{
			return _Label662;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label662 = value;
		}
	}

	internal virtual Label Label663
	{
		get
		{
			return _Label663;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label663 = value;
		}
	}

	internal virtual TrackBar RevDirectVolTrabar
	{
		get
		{
			return _RevDirectVolTrabar;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevDirectVolTrabar = value;
		}
	}

	internal virtual TrackBar RevVolTrabar
	{
		get
		{
			return _RevVolTrabar;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevVolTrabar = value;
		}
	}

	internal virtual Label Label567
	{
		get
		{
			return _Label567;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label567 = value;
		}
	}

	internal virtual Label Label568
	{
		get
		{
			return _Label568;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label568 = value;
		}
	}

	internal virtual Label Label569
	{
		get
		{
			return _Label569;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label569 = value;
		}
	}

	internal virtual Panel Panel70
	{
		get
		{
			return _Panel70;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel70 = value;
		}
	}

	internal virtual ButtonX RevHS5
	{
		get
		{
			return _RevHS5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevHS5 = value;
		}
	}

	internal virtual ButtonX RevLS5
	{
		get
		{
			return _RevLS5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevLS5 = value;
		}
	}

	internal virtual ButtonX RevP5
	{
		get
		{
			return _RevP5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevP5 = value;
		}
	}

	internal virtual Label Label570
	{
		get
		{
			return _Label570;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label570 = value;
		}
	}

	internal virtual Label Label571
	{
		get
		{
			return _Label571;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label571 = value;
		}
	}

	internal virtual Label Label572
	{
		get
		{
			return _Label572;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label572 = value;
		}
	}

	internal virtual DomainUpDown RevFreqEQ5
	{
		get
		{
			return _RevFreqEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevFreqEQ5 = value;
		}
	}

	internal virtual DomainUpDown RevGainEQ5
	{
		get
		{
			return _RevGainEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevGainEQ5 = value;
		}
	}

	internal virtual DomainUpDown RevQEQ5
	{
		get
		{
			return _RevQEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevQEQ5 = value;
		}
	}

	internal virtual TrackBar RevTrackBar5
	{
		get
		{
			return _RevTrackBar5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevTrackBar5 = value;
		}
	}

	internal virtual Label Label573
	{
		get
		{
			return _Label573;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label573 = value;
		}
	}

	internal virtual Panel Panel71
	{
		get
		{
			return _Panel71;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel71 = value;
		}
	}

	internal virtual ButtonX RevHS2
	{
		get
		{
			return _RevHS2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevHS2 = value;
		}
	}

	internal virtual ButtonX RevLS2
	{
		get
		{
			return _RevLS2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevLS2 = value;
		}
	}

	internal virtual ButtonX RevP2
	{
		get
		{
			return _RevP2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevP2 = value;
		}
	}

	internal virtual Label Label574
	{
		get
		{
			return _Label574;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label574 = value;
		}
	}

	internal virtual Label Label575
	{
		get
		{
			return _Label575;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label575 = value;
		}
	}

	internal virtual Label Label576
	{
		get
		{
			return _Label576;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label576 = value;
		}
	}

	internal virtual DomainUpDown RevFreqEQ2
	{
		get
		{
			return _RevFreqEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevFreqEQ2 = value;
		}
	}

	internal virtual DomainUpDown RevGainEQ2
	{
		get
		{
			return _RevGainEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevGainEQ2 = value;
		}
	}

	internal virtual DomainUpDown RevQEQ2
	{
		get
		{
			return _RevQEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevQEQ2 = value;
		}
	}

	internal virtual TrackBar RevTrackBar2
	{
		get
		{
			return _RevTrackBar2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevTrackBar2 = value;
		}
	}

	internal virtual Panel Panel72
	{
		get
		{
			return _Panel72;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel72 = value;
		}
	}

	internal virtual ButtonX RevHS1
	{
		get
		{
			return _RevHS1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevHS1 = value;
		}
	}

	internal virtual ButtonX RevLS1
	{
		get
		{
			return _RevLS1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevLS1 = value;
		}
	}

	internal virtual ButtonX RevP1
	{
		get
		{
			return _RevP1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevP1 = value;
		}
	}

	internal virtual Label Label577
	{
		get
		{
			return _Label577;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label577 = value;
		}
	}

	internal virtual Label Label608
	{
		get
		{
			return _Label608;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label608 = value;
		}
	}

	internal virtual Label Label609
	{
		get
		{
			return _Label609;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label609 = value;
		}
	}

	internal virtual DomainUpDown RevFreqEQ1
	{
		get
		{
			return _RevFreqEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevFreqEQ1 = value;
		}
	}

	internal virtual DomainUpDown RevGainEQ1
	{
		get
		{
			return _RevGainEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevGainEQ1 = value;
		}
	}

	internal virtual DomainUpDown RevQEQ1
	{
		get
		{
			return _RevQEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevQEQ1 = value;
		}
	}

	internal virtual TrackBar RevTrackBar1
	{
		get
		{
			return _RevTrackBar1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevTrackBar1 = value;
		}
	}

	internal virtual Panel Panel73
	{
		get
		{
			return _Panel73;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel73 = value;
		}
	}

	internal virtual ButtonX RevHS3
	{
		get
		{
			return _RevHS3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevHS3 = value;
		}
	}

	internal virtual ButtonX RevLS3
	{
		get
		{
			return _RevLS3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevLS3 = value;
		}
	}

	internal virtual ButtonX RevP3
	{
		get
		{
			return _RevP3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevP3 = value;
		}
	}

	internal virtual Label Label610
	{
		get
		{
			return _Label610;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label610 = value;
		}
	}

	internal virtual Label Label611
	{
		get
		{
			return _Label611;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label611 = value;
		}
	}

	internal virtual Label Label612
	{
		get
		{
			return _Label612;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label612 = value;
		}
	}

	internal virtual DomainUpDown RevFreqEQ3
	{
		get
		{
			return _RevFreqEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevFreqEQ3 = value;
		}
	}

	internal virtual DomainUpDown RevGainEQ3
	{
		get
		{
			return _RevGainEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevGainEQ3 = value;
		}
	}

	internal virtual DomainUpDown RevQEQ3
	{
		get
		{
			return _RevQEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevQEQ3 = value;
		}
	}

	internal virtual TrackBar RevTrackBar3
	{
		get
		{
			return _RevTrackBar3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevTrackBar3 = value;
		}
	}

	internal virtual Panel Panel74
	{
		get
		{
			return _Panel74;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel74 = value;
		}
	}

	internal virtual ButtonX RevHS4
	{
		get
		{
			return _RevHS4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevHS4 = value;
		}
	}

	internal virtual ButtonX RevLS4
	{
		get
		{
			return _RevLS4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevLS4 = value;
		}
	}

	internal virtual ButtonX RevP4
	{
		get
		{
			return _RevP4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevP4 = value;
		}
	}

	internal virtual Label Label613
	{
		get
		{
			return _Label613;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label613 = value;
		}
	}

	internal virtual Label Label614
	{
		get
		{
			return _Label614;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label614 = value;
		}
	}

	internal virtual Label Label615
	{
		get
		{
			return _Label615;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label615 = value;
		}
	}

	internal virtual DomainUpDown RevFreqEQ4
	{
		get
		{
			return _RevFreqEQ4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevFreqEQ4 = value;
		}
	}

	internal virtual DomainUpDown RevGainEQ4
	{
		get
		{
			return _RevGainEQ4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevGainEQ4 = value;
		}
	}

	internal virtual DomainUpDown RevQEQ4
	{
		get
		{
			return _RevQEQ4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevQEQ4 = value;
		}
	}

	internal virtual TrackBar RevTrackBar4
	{
		get
		{
			return _RevTrackBar4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevTrackBar4 = value;
		}
	}

	internal virtual SuperTabControlPanel SuperTabControlPanel5
	{
		get
		{
			return _SuperTabControlPanel5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SuperTabControlPanel5 = value;
		}
	}

	internal virtual PictureBox EchoPic
	{
		get
		{
			return _EchoPic;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoPic = value;
		}
	}

	internal virtual Label EchoDelayRLable
	{
		get
		{
			return _EchoDelayRLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoDelayRLable = value;
		}
	}

	internal virtual Label EchoPreDelayRLable
	{
		get
		{
			return _EchoPreDelayRLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoPreDelayRLable = value;
		}
	}

	internal virtual Label Label633
	{
		get
		{
			return _Label633;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label633 = value;
		}
	}

	internal virtual Label EchoRepeatLable
	{
		get
		{
			return _EchoRepeatLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoRepeatLable = value;
		}
	}

	internal virtual Label EchoDelayLable
	{
		get
		{
			return _EchoDelayLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoDelayLable = value;
		}
	}

	internal virtual Label EchoPreDelayLable
	{
		get
		{
			return _EchoPreDelayLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoPreDelayLable = value;
		}
	}

	internal virtual Label Label641
	{
		get
		{
			return _Label641;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label641 = value;
		}
	}

	internal virtual Label Label637
	{
		get
		{
			return _Label637;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label637 = value;
		}
	}

	internal virtual Label Label638
	{
		get
		{
			return _Label638;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label638 = value;
		}
	}

	internal virtual Label Label639
	{
		get
		{
			return _Label639;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label639 = value;
		}
	}

	internal virtual Label Label640
	{
		get
		{
			return _Label640;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label640 = value;
		}
	}

	internal virtual TrackBar EchoDelayR
	{
		get
		{
			return _EchoDelayR;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoDelayR = value;
		}
	}

	internal virtual TrackBar EchoPreDelayR
	{
		get
		{
			return _EchoPreDelayR;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoPreDelayR = value;
		}
	}

	internal virtual TrackBar EchoRepeat
	{
		get
		{
			return _EchoRepeat;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoRepeat = value;
		}
	}

	internal virtual TrackBar EchoDelay
	{
		get
		{
			return _EchoDelay;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoDelay = value;
		}
	}

	internal virtual TrackBar EchoPreDelay
	{
		get
		{
			return _EchoPreDelay;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoPreDelay = value;
		}
	}

	internal virtual Label EchoLpfLable
	{
		get
		{
			return _EchoLpfLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoLpfLable = value;
		}
	}

	internal virtual Label EchoHpfLable
	{
		get
		{
			return _EchoHpfLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoHpfLable = value;
		}
	}

	internal virtual Label Label630
	{
		get
		{
			return _Label630;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label630 = value;
		}
	}

	internal virtual TrackBar EchoLpf
	{
		get
		{
			return _EchoLpf;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoLpf = value;
		}
	}

	internal virtual Label Label629
	{
		get
		{
			return _Label629;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label629 = value;
		}
	}

	internal virtual TrackBar EchoHpf
	{
		get
		{
			return _EchoHpf;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoHpf = value;
		}
	}

	internal virtual Label EchoDirectVolPhase
	{
		get
		{
			return _EchoDirectVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoDirectVolPhase = value;
		}
	}

	internal virtual Label EchoDirectVolLable
	{
		get
		{
			return _EchoDirectVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoDirectVolLable = value;
		}
	}

	internal virtual Label EchoVolPhase
	{
		get
		{
			return _EchoVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoVolPhase = value;
		}
	}

	internal virtual Label EchoVolLable
	{
		get
		{
			return _EchoVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoVolLable = value;
		}
	}

	internal virtual Label Label620
	{
		get
		{
			return _Label620;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label620 = value;
		}
	}

	internal virtual Label Label621
	{
		get
		{
			return _Label621;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label621 = value;
		}
	}

	internal virtual TrackBar EchoDirectVolTrabar
	{
		get
		{
			return _EchoDirectVolTrabar;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoDirectVolTrabar = value;
		}
	}

	internal virtual TrackBar EchoVolTrabar
	{
		get
		{
			return _EchoVolTrabar;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoVolTrabar = value;
		}
	}

	internal virtual Label Label548
	{
		get
		{
			return _Label548;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label548 = value;
		}
	}

	internal virtual Label Label549
	{
		get
		{
			return _Label549;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label549 = value;
		}
	}

	internal virtual Label Label550
	{
		get
		{
			return _Label550;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label550 = value;
		}
	}

	internal virtual Panel Panel62
	{
		get
		{
			return _Panel62;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel62 = value;
		}
	}

	internal virtual ButtonX EchoHS5
	{
		get
		{
			return _EchoHS5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoHS5 = value;
		}
	}

	internal virtual ButtonX EchoLS5
	{
		get
		{
			return _EchoLS5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoLS5 = value;
		}
	}

	internal virtual ButtonX EchoP5
	{
		get
		{
			return _EchoP5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoP5 = value;
		}
	}

	internal virtual Label Label551
	{
		get
		{
			return _Label551;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label551 = value;
		}
	}

	internal virtual Label Label552
	{
		get
		{
			return _Label552;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label552 = value;
		}
	}

	internal virtual Label Label553
	{
		get
		{
			return _Label553;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label553 = value;
		}
	}

	internal virtual DomainUpDown EchoFreqEQ5
	{
		get
		{
			return _EchoFreqEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoFreqEQ5 = value;
		}
	}

	internal virtual DomainUpDown EchoGainEQ5
	{
		get
		{
			return _EchoGainEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoGainEQ5 = value;
		}
	}

	internal virtual DomainUpDown EchoQEQ5
	{
		get
		{
			return _EchoQEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoQEQ5 = value;
		}
	}

	internal virtual TrackBar EchoTrackBar5
	{
		get
		{
			return _EchoTrackBar5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoTrackBar5 = value;
		}
	}

	internal virtual Label Label554
	{
		get
		{
			return _Label554;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label554 = value;
		}
	}

	internal virtual Panel Panel66
	{
		get
		{
			return _Panel66;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel66 = value;
		}
	}

	internal virtual ButtonX EchoHS2
	{
		get
		{
			return _EchoHS2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoHS2 = value;
		}
	}

	internal virtual ButtonX EchoLS2
	{
		get
		{
			return _EchoLS2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoLS2 = value;
		}
	}

	internal virtual ButtonX EchoP2
	{
		get
		{
			return _EchoP2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoP2 = value;
		}
	}

	internal virtual Label Label555
	{
		get
		{
			return _Label555;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label555 = value;
		}
	}

	internal virtual Label Label556
	{
		get
		{
			return _Label556;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label556 = value;
		}
	}

	internal virtual Label Label557
	{
		get
		{
			return _Label557;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label557 = value;
		}
	}

	internal virtual DomainUpDown EchoFreqEQ2
	{
		get
		{
			return _EchoFreqEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoFreqEQ2 = value;
		}
	}

	internal virtual DomainUpDown EchoGainEQ2
	{
		get
		{
			return _EchoGainEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoGainEQ2 = value;
		}
	}

	internal virtual DomainUpDown EchoQEQ2
	{
		get
		{
			return _EchoQEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoQEQ2 = value;
		}
	}

	internal virtual TrackBar EchoTrackBar2
	{
		get
		{
			return _EchoTrackBar2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoTrackBar2 = value;
		}
	}

	internal virtual Panel Panel67
	{
		get
		{
			return _Panel67;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel67 = value;
		}
	}

	internal virtual ButtonX EchoHS1
	{
		get
		{
			return _EchoHS1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoHS1 = value;
		}
	}

	internal virtual ButtonX EchoLS1
	{
		get
		{
			return _EchoLS1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoLS1 = value;
		}
	}

	internal virtual ButtonX EchoP1
	{
		get
		{
			return _EchoP1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoP1 = value;
		}
	}

	internal virtual Label Label558
	{
		get
		{
			return _Label558;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label558 = value;
		}
	}

	internal virtual Label Label559
	{
		get
		{
			return _Label559;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label559 = value;
		}
	}

	internal virtual Label Label560
	{
		get
		{
			return _Label560;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label560 = value;
		}
	}

	internal virtual DomainUpDown EchoFreqEQ1
	{
		get
		{
			return _EchoFreqEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoFreqEQ1 = value;
		}
	}

	internal virtual DomainUpDown EchoGainEQ1
	{
		get
		{
			return _EchoGainEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoGainEQ1 = value;
		}
	}

	internal virtual DomainUpDown EchoQEQ1
	{
		get
		{
			return _EchoQEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoQEQ1 = value;
		}
	}

	internal virtual TrackBar EchoTrackBar1
	{
		get
		{
			return _EchoTrackBar1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoTrackBar1 = value;
		}
	}

	internal virtual Panel Panel68
	{
		get
		{
			return _Panel68;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel68 = value;
		}
	}

	internal virtual ButtonX EchoHS3
	{
		get
		{
			return _EchoHS3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoHS3 = value;
		}
	}

	internal virtual ButtonX EchoLS3
	{
		get
		{
			return _EchoLS3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoLS3 = value;
		}
	}

	internal virtual ButtonX EchoP3
	{
		get
		{
			return _EchoP3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoP3 = value;
		}
	}

	internal virtual Label Label561
	{
		get
		{
			return _Label561;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label561 = value;
		}
	}

	internal virtual Label Label562
	{
		get
		{
			return _Label562;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label562 = value;
		}
	}

	internal virtual Label Label563
	{
		get
		{
			return _Label563;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label563 = value;
		}
	}

	internal virtual DomainUpDown EchoFreqEQ3
	{
		get
		{
			return _EchoFreqEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoFreqEQ3 = value;
		}
	}

	internal virtual DomainUpDown EchoGainEQ3
	{
		get
		{
			return _EchoGainEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoGainEQ3 = value;
		}
	}

	internal virtual DomainUpDown EchoQEQ3
	{
		get
		{
			return _EchoQEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoQEQ3 = value;
		}
	}

	internal virtual TrackBar EchoTrackBar3
	{
		get
		{
			return _EchoTrackBar3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoTrackBar3 = value;
		}
	}

	internal virtual Panel Panel69
	{
		get
		{
			return _Panel69;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel69 = value;
		}
	}

	internal virtual ButtonX EchoHS4
	{
		get
		{
			return _EchoHS4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoHS4 = value;
		}
	}

	internal virtual ButtonX EchoLS4
	{
		get
		{
			return _EchoLS4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoLS4 = value;
		}
	}

	internal virtual ButtonX EchoP4
	{
		get
		{
			return _EchoP4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoP4 = value;
		}
	}

	internal virtual Label Label564
	{
		get
		{
			return _Label564;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label564 = value;
		}
	}

	internal virtual Label Label565
	{
		get
		{
			return _Label565;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label565 = value;
		}
	}

	internal virtual Label Label566
	{
		get
		{
			return _Label566;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label566 = value;
		}
	}

	internal virtual DomainUpDown EchoFreqEQ4
	{
		get
		{
			return _EchoFreqEQ4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoFreqEQ4 = value;
		}
	}

	internal virtual DomainUpDown EchoGainEQ4
	{
		get
		{
			return _EchoGainEQ4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoGainEQ4 = value;
		}
	}

	internal virtual DomainUpDown EchoQEQ4
	{
		get
		{
			return _EchoQEQ4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoQEQ4 = value;
		}
	}

	internal virtual TrackBar EchoTrackBar4
	{
		get
		{
			return _EchoTrackBar4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoTrackBar4 = value;
		}
	}

	internal virtual SuperTabControlPanel SuperTabControlPanel4
	{
		get
		{
			return _SuperTabControlPanel4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SuperTabControlPanel4 = value;
		}
	}

	internal virtual PictureBox MicBPic
	{
		get
		{
			return _MicBPic;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicBPic = value;
		}
	}

	internal virtual GroupBox GroupBox38
	{
		get
		{
			return _GroupBox38;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_GroupBox38 = value;
		}
	}

	internal virtual Label NoiseBDecTimeLable
	{
		get
		{
			return _NoiseBDecTimeLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_NoiseBDecTimeLable = value;
		}
	}

	internal virtual Label NoiseBAttTimeLable
	{
		get
		{
			return _NoiseBAttTimeLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_NoiseBAttTimeLable = value;
		}
	}

	internal virtual Label NoiseBThresholdLable
	{
		get
		{
			return _NoiseBThresholdLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_NoiseBThresholdLable = value;
		}
	}

	internal virtual Label Label273
	{
		get
		{
			return _Label273;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label273 = value;
		}
	}

	internal virtual Label Label274
	{
		get
		{
			return _Label274;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label274 = value;
		}
	}

	internal virtual Label Label275
	{
		get
		{
			return _Label275;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label275 = value;
		}
	}

	internal virtual TrackBar NoiseBDecTime
	{
		get
		{
			return _NoiseBDecTime;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_NoiseBDecTime = value;
		}
	}

	internal virtual TrackBar NoiseBAttTime
	{
		get
		{
			return _NoiseBAttTime;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_NoiseBAttTime = value;
		}
	}

	internal virtual TrackBar NoiseBThreshold
	{
		get
		{
			return _NoiseBThreshold;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_NoiseBThreshold = value;
		}
	}

	internal virtual GroupBox GroupBox39
	{
		get
		{
			return _GroupBox39;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_GroupBox39 = value;
		}
	}

	internal virtual Label MicBRatioLable
	{
		get
		{
			return _MicBRatioLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicBRatioLable = value;
		}
	}

	internal virtual Label MicBDecTimeLable
	{
		get
		{
			return _MicBDecTimeLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicBDecTimeLable = value;
		}
	}

	internal virtual Label MicBAttTimeLable
	{
		get
		{
			return _MicBAttTimeLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicBAttTimeLable = value;
		}
	}

	internal virtual Label MicBThresholdLable
	{
		get
		{
			return _MicBThresholdLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicBThresholdLable = value;
		}
	}

	internal virtual Label Label280
	{
		get
		{
			return _Label280;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label280 = value;
		}
	}

	internal virtual Label Label345
	{
		get
		{
			return _Label345;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label345 = value;
		}
	}

	internal virtual TrackBar MicBRatio
	{
		get
		{
			return _MicBRatio;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicBRatio = value;
		}
	}

	internal virtual TrackBar MicBDecTime
	{
		get
		{
			return _MicBDecTime;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicBDecTime = value;
		}
	}

	internal virtual Label Label346
	{
		get
		{
			return _Label346;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label346 = value;
		}
	}

	internal virtual TrackBar MicBAttTime
	{
		get
		{
			return _MicBAttTime;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicBAttTime = value;
		}
	}

	internal virtual TrackBar MicBThreshold
	{
		get
		{
			return _MicBThreshold;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicBThreshold = value;
		}
	}

	internal virtual Label Label347
	{
		get
		{
			return _Label347;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label347 = value;
		}
	}

	internal virtual Label Label348
	{
		get
		{
			return _Label348;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label348 = value;
		}
	}

	internal virtual ComboBox MicB_FBE
	{
		get
		{
			return _MicB_FBE;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			EventHandler eventHandler = MicB_FBE_SelectedIndexChanged;
			if (_MicB_FBE != null)
			{
				_MicB_FBE.SelectedIndexChanged -= eventHandler;
			}
			_MicB_FBE = value;
			if (_MicB_FBE != null)
			{
				_MicB_FBE.SelectedIndexChanged += eventHandler;
			}
		}
	}

	internal virtual Label MicBHpfLable
	{
		get
		{
			return _MicBHpfLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicBHpfLable = value;
		}
	}

	internal virtual Label Label350
	{
		get
		{
			return _Label350;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label350 = value;
		}
	}

	internal virtual TrackBar MicBHpf
	{
		get
		{
			return _MicBHpf;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicBHpf = value;
		}
	}

	internal virtual Label Label351
	{
		get
		{
			return _Label351;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label351 = value;
		}
	}

	internal virtual PanelEx PanelEx2
	{
		get
		{
			return _PanelEx2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_PanelEx2 = value;
		}
	}

	internal virtual Panel Panel34
	{
		get
		{
			return _Panel34;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel34 = value;
		}
	}

	internal virtual Panel Panel37
	{
		get
		{
			return _Panel37;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel37 = value;
		}
	}

	internal virtual Panel Panel42
	{
		get
		{
			return _Panel42;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel42 = value;
		}
	}

	internal virtual ButtonX Mic2HS10
	{
		get
		{
			return _Mic2HS10;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2HS10 = value;
		}
	}

	internal virtual ButtonX Mic2LS10
	{
		get
		{
			return _Mic2LS10;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2LS10 = value;
		}
	}

	internal virtual ButtonX Mic2P10
	{
		get
		{
			return _Mic2P10;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2P10 = value;
		}
	}

	internal virtual Label Label315
	{
		get
		{
			return _Label315;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label315 = value;
		}
	}

	internal virtual Label Label316
	{
		get
		{
			return _Label316;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label316 = value;
		}
	}

	internal virtual Label Label317
	{
		get
		{
			return _Label317;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label317 = value;
		}
	}

	internal virtual DomainUpDown Mic2FreqEQ10
	{
		get
		{
			return _Mic2FreqEQ10;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2FreqEQ10 = value;
		}
	}

	internal virtual DomainUpDown Mic2GainEQ10
	{
		get
		{
			return _Mic2GainEQ10;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2GainEQ10 = value;
		}
	}

	internal virtual DomainUpDown Mic2QEQ10
	{
		get
		{
			return _Mic2QEQ10;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2QEQ10 = value;
		}
	}

	internal virtual TrackBar Mic2TrackBar10
	{
		get
		{
			return _Mic2TrackBar10;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2TrackBar10 = value;
		}
	}

	internal virtual Panel Panel43
	{
		get
		{
			return _Panel43;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel43 = value;
		}
	}

	internal virtual ButtonX Mic2HS5
	{
		get
		{
			return _Mic2HS5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2HS5 = value;
		}
	}

	internal virtual ButtonX Mic2LS5
	{
		get
		{
			return _Mic2LS5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2LS5 = value;
		}
	}

	internal virtual ButtonX Mic2P5
	{
		get
		{
			return _Mic2P5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2P5 = value;
		}
	}

	internal virtual Label Label318
	{
		get
		{
			return _Label318;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label318 = value;
		}
	}

	internal virtual Label Label319
	{
		get
		{
			return _Label319;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label319 = value;
		}
	}

	internal virtual Label Label320
	{
		get
		{
			return _Label320;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label320 = value;
		}
	}

	internal virtual DomainUpDown Mic2FreqEQ5
	{
		get
		{
			return _Mic2FreqEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2FreqEQ5 = value;
		}
	}

	internal virtual DomainUpDown Mic2GainEQ5
	{
		get
		{
			return _Mic2GainEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2GainEQ5 = value;
		}
	}

	internal virtual DomainUpDown Mic2QEQ5
	{
		get
		{
			return _Mic2QEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2QEQ5 = value;
		}
	}

	internal virtual TrackBar Mic2TrackBar5
	{
		get
		{
			return _Mic2TrackBar5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2TrackBar5 = value;
		}
	}

	internal virtual Panel Panel44
	{
		get
		{
			return _Panel44;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel44 = value;
		}
	}

	internal virtual ButtonX Mic2HS2
	{
		get
		{
			return _Mic2HS2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2HS2 = value;
		}
	}

	internal virtual ButtonX Mic2LS2
	{
		get
		{
			return _Mic2LS2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2LS2 = value;
		}
	}

	internal virtual ButtonX Mic2P2
	{
		get
		{
			return _Mic2P2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2P2 = value;
		}
	}

	internal virtual Label Label321
	{
		get
		{
			return _Label321;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label321 = value;
		}
	}

	internal virtual Label Label322
	{
		get
		{
			return _Label322;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label322 = value;
		}
	}

	internal virtual Label Label323
	{
		get
		{
			return _Label323;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label323 = value;
		}
	}

	internal virtual DomainUpDown Mic2FreqEQ2
	{
		get
		{
			return _Mic2FreqEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2FreqEQ2 = value;
		}
	}

	internal virtual DomainUpDown Mic2GainEQ2
	{
		get
		{
			return _Mic2GainEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2GainEQ2 = value;
		}
	}

	internal virtual DomainUpDown Mic2QEQ2
	{
		get
		{
			return _Mic2QEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2QEQ2 = value;
		}
	}

	internal virtual TrackBar Mic2TrackBar2
	{
		get
		{
			return _Mic2TrackBar2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2TrackBar2 = value;
		}
	}

	internal virtual Panel Panel45
	{
		get
		{
			return _Panel45;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel45 = value;
		}
	}

	internal virtual ButtonX Mic2HS8
	{
		get
		{
			return _Mic2HS8;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2HS8 = value;
		}
	}

	internal virtual ButtonX Mic2LS8
	{
		get
		{
			return _Mic2LS8;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2LS8 = value;
		}
	}

	internal virtual ButtonX Mic2P8
	{
		get
		{
			return _Mic2P8;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2P8 = value;
		}
	}

	internal virtual Label Label324
	{
		get
		{
			return _Label324;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label324 = value;
		}
	}

	internal virtual Label Label325
	{
		get
		{
			return _Label325;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label325 = value;
		}
	}

	internal virtual Label Label326
	{
		get
		{
			return _Label326;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label326 = value;
		}
	}

	internal virtual DomainUpDown Mic2FreqEQ8
	{
		get
		{
			return _Mic2FreqEQ8;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2FreqEQ8 = value;
		}
	}

	internal virtual DomainUpDown Mic2GainEQ8
	{
		get
		{
			return _Mic2GainEQ8;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2GainEQ8 = value;
		}
	}

	internal virtual DomainUpDown Mic2QEQ8
	{
		get
		{
			return _Mic2QEQ8;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2QEQ8 = value;
		}
	}

	internal virtual TrackBar Mic2TrackBar8
	{
		get
		{
			return _Mic2TrackBar8;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2TrackBar8 = value;
		}
	}

	internal virtual Panel Panel46
	{
		get
		{
			return _Panel46;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel46 = value;
		}
	}

	internal virtual ButtonX Mic2HS6
	{
		get
		{
			return _Mic2HS6;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2HS6 = value;
		}
	}

	internal virtual ButtonX Mic2LS6
	{
		get
		{
			return _Mic2LS6;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2LS6 = value;
		}
	}

	internal virtual ButtonX Mic2P6
	{
		get
		{
			return _Mic2P6;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2P6 = value;
		}
	}

	internal virtual Label Label327
	{
		get
		{
			return _Label327;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label327 = value;
		}
	}

	internal virtual Label Label328
	{
		get
		{
			return _Label328;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label328 = value;
		}
	}

	internal virtual Label Label329
	{
		get
		{
			return _Label329;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label329 = value;
		}
	}

	internal virtual DomainUpDown Mic2FreqEQ6
	{
		get
		{
			return _Mic2FreqEQ6;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2FreqEQ6 = value;
		}
	}

	internal virtual DomainUpDown Mic2GainEQ6
	{
		get
		{
			return _Mic2GainEQ6;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2GainEQ6 = value;
		}
	}

	internal virtual DomainUpDown Mic2QEQ6
	{
		get
		{
			return _Mic2QEQ6;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2QEQ6 = value;
		}
	}

	internal virtual TrackBar Mic2TrackBar6
	{
		get
		{
			return _Mic2TrackBar6;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2TrackBar6 = value;
		}
	}

	internal virtual Panel Panel47
	{
		get
		{
			return _Panel47;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel47 = value;
		}
	}

	internal virtual ButtonX Mic2HS1
	{
		get
		{
			return _Mic2HS1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2HS1 = value;
		}
	}

	internal virtual ButtonX Mic2LS1
	{
		get
		{
			return _Mic2LS1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2LS1 = value;
		}
	}

	internal virtual ButtonX Mic2P1
	{
		get
		{
			return _Mic2P1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2P1 = value;
		}
	}

	internal virtual Label Label330
	{
		get
		{
			return _Label330;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label330 = value;
		}
	}

	internal virtual Label Label331
	{
		get
		{
			return _Label331;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label331 = value;
		}
	}

	internal virtual Label Label332
	{
		get
		{
			return _Label332;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label332 = value;
		}
	}

	internal virtual DomainUpDown Mic2FreqEQ1
	{
		get
		{
			return _Mic2FreqEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2FreqEQ1 = value;
		}
	}

	internal virtual DomainUpDown Mic2GainEQ1
	{
		get
		{
			return _Mic2GainEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2GainEQ1 = value;
		}
	}

	internal virtual DomainUpDown Mic2QEQ1
	{
		get
		{
			return _Mic2QEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2QEQ1 = value;
		}
	}

	internal virtual TrackBar Mic2TrackBar1
	{
		get
		{
			return _Mic2TrackBar1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2TrackBar1 = value;
		}
	}

	internal virtual Panel Panel48
	{
		get
		{
			return _Panel48;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel48 = value;
		}
	}

	internal virtual ButtonX Mic2HS3
	{
		get
		{
			return _Mic2HS3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2HS3 = value;
		}
	}

	internal virtual ButtonX Mic2LS3
	{
		get
		{
			return _Mic2LS3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2LS3 = value;
		}
	}

	internal virtual ButtonX Mic2P3
	{
		get
		{
			return _Mic2P3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2P3 = value;
		}
	}

	internal virtual Label Label333
	{
		get
		{
			return _Label333;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label333 = value;
		}
	}

	internal virtual Label Label334
	{
		get
		{
			return _Label334;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label334 = value;
		}
	}

	internal virtual Label Label335
	{
		get
		{
			return _Label335;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label335 = value;
		}
	}

	internal virtual DomainUpDown Mic2FreqEQ3
	{
		get
		{
			return _Mic2FreqEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2FreqEQ3 = value;
		}
	}

	internal virtual DomainUpDown Mic2GainEQ3
	{
		get
		{
			return _Mic2GainEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2GainEQ3 = value;
		}
	}

	internal virtual DomainUpDown Mic2QEQ3
	{
		get
		{
			return _Mic2QEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2QEQ3 = value;
		}
	}

	internal virtual TrackBar Mic2TrackBar3
	{
		get
		{
			return _Mic2TrackBar3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2TrackBar3 = value;
		}
	}

	internal virtual Panel Panel49
	{
		get
		{
			return _Panel49;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel49 = value;
		}
	}

	internal virtual ButtonX Mic2HS7
	{
		get
		{
			return _Mic2HS7;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2HS7 = value;
		}
	}

	internal virtual ButtonX Mic2LS7
	{
		get
		{
			return _Mic2LS7;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2LS7 = value;
		}
	}

	internal virtual ButtonX Mic2P7
	{
		get
		{
			return _Mic2P7;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2P7 = value;
		}
	}

	internal virtual Label Label336
	{
		get
		{
			return _Label336;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label336 = value;
		}
	}

	internal virtual Label Label337
	{
		get
		{
			return _Label337;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label337 = value;
		}
	}

	internal virtual Label Label338
	{
		get
		{
			return _Label338;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label338 = value;
		}
	}

	internal virtual DomainUpDown Mic2FreqEQ7
	{
		get
		{
			return _Mic2FreqEQ7;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2FreqEQ7 = value;
		}
	}

	internal virtual DomainUpDown Mic2GainEQ7
	{
		get
		{
			return _Mic2GainEQ7;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2GainEQ7 = value;
		}
	}

	internal virtual DomainUpDown Mic2QEQ7
	{
		get
		{
			return _Mic2QEQ7;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2QEQ7 = value;
		}
	}

	internal virtual TrackBar Mic2TrackBar7
	{
		get
		{
			return _Mic2TrackBar7;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2TrackBar7 = value;
		}
	}

	internal virtual Panel Panel50
	{
		get
		{
			return _Panel50;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel50 = value;
		}
	}

	internal virtual ButtonX Mic2HS9
	{
		get
		{
			return _Mic2HS9;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2HS9 = value;
		}
	}

	internal virtual ButtonX Mic2LS9
	{
		get
		{
			return _Mic2LS9;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2LS9 = value;
		}
	}

	internal virtual ButtonX Mic2P9
	{
		get
		{
			return _Mic2P9;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2P9 = value;
		}
	}

	internal virtual Label Label339
	{
		get
		{
			return _Label339;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label339 = value;
		}
	}

	internal virtual Label Label340
	{
		get
		{
			return _Label340;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label340 = value;
		}
	}

	internal virtual Label Label341
	{
		get
		{
			return _Label341;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label341 = value;
		}
	}

	internal virtual DomainUpDown Mic2FreqEQ9
	{
		get
		{
			return _Mic2FreqEQ9;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2FreqEQ9 = value;
		}
	}

	internal virtual DomainUpDown Mic2GainEQ9
	{
		get
		{
			return _Mic2GainEQ9;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2GainEQ9 = value;
		}
	}

	internal virtual DomainUpDown Mic2QEQ9
	{
		get
		{
			return _Mic2QEQ9;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2QEQ9 = value;
		}
	}

	internal virtual TrackBar Mic2TrackBar9
	{
		get
		{
			return _Mic2TrackBar9;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2TrackBar9 = value;
		}
	}

	internal virtual Panel Panel51
	{
		get
		{
			return _Panel51;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel51 = value;
		}
	}

	internal virtual ButtonX Mic2HS4
	{
		get
		{
			return _Mic2HS4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2HS4 = value;
		}
	}

	internal virtual ButtonX Mic2LS4
	{
		get
		{
			return _Mic2LS4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2LS4 = value;
		}
	}

	internal virtual ButtonX Mic2P4
	{
		get
		{
			return _Mic2P4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2P4 = value;
		}
	}

	internal virtual Label Label342
	{
		get
		{
			return _Label342;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label342 = value;
		}
	}

	internal virtual Label Label343
	{
		get
		{
			return _Label343;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label343 = value;
		}
	}

	internal virtual Label Label344
	{
		get
		{
			return _Label344;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label344 = value;
		}
	}

	internal virtual DomainUpDown Mic2FreqEQ4
	{
		get
		{
			return _Mic2FreqEQ4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2FreqEQ4 = value;
		}
	}

	internal virtual DomainUpDown Mic2GainEQ4
	{
		get
		{
			return _Mic2GainEQ4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2GainEQ4 = value;
		}
	}

	internal virtual DomainUpDown Mic2QEQ4
	{
		get
		{
			return _Mic2QEQ4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2QEQ4 = value;
		}
	}

	internal virtual TrackBar Mic2TrackBar4
	{
		get
		{
			return _Mic2TrackBar4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2TrackBar4 = value;
		}
	}

	internal virtual Label Label281
	{
		get
		{
			return _Label281;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label281 = value;
		}
	}

	internal virtual Label Label282
	{
		get
		{
			return _Label282;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label282 = value;
		}
	}

	internal virtual Label Label284
	{
		get
		{
			return _Label284;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label284 = value;
		}
	}

	internal virtual Label Label283
	{
		get
		{
			return _Label283;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label283 = value;
		}
	}

	internal virtual SuperTabControlPanel SuperTabControlPanel3
	{
		get
		{
			return _SuperTabControlPanel3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SuperTabControlPanel3 = value;
		}
	}

	internal virtual PictureBox MicAPic
	{
		get
		{
			return _MicAPic;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicAPic = value;
		}
	}

	internal virtual GroupBox GroupBox40
	{
		get
		{
			return _GroupBox40;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_GroupBox40 = value;
		}
	}

	internal virtual Label NoiseADecTimeLable
	{
		get
		{
			return _NoiseADecTimeLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_NoiseADecTimeLable = value;
		}
	}

	internal virtual Label NoiseAAttTimeLable
	{
		get
		{
			return _NoiseAAttTimeLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_NoiseAAttTimeLable = value;
		}
	}

	internal virtual Label NoiseAThresholdLable
	{
		get
		{
			return _NoiseAThresholdLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_NoiseAThresholdLable = value;
		}
	}

	internal virtual Label Label262
	{
		get
		{
			return _Label262;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label262 = value;
		}
	}

	internal virtual Label Label263
	{
		get
		{
			return _Label263;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label263 = value;
		}
	}

	internal virtual Label Label264
	{
		get
		{
			return _Label264;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label264 = value;
		}
	}

	internal virtual TrackBar NoiseADecTime
	{
		get
		{
			return _NoiseADecTime;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_NoiseADecTime = value;
		}
	}

	internal virtual TrackBar NoiseAAttTime
	{
		get
		{
			return _NoiseAAttTime;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_NoiseAAttTime = value;
		}
	}

	internal virtual TrackBar NoiseAThreshold
	{
		get
		{
			return _NoiseAThreshold;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_NoiseAThreshold = value;
		}
	}

	internal virtual GroupBox GroupBox41
	{
		get
		{
			return _GroupBox41;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_GroupBox41 = value;
		}
	}

	internal virtual Label MicARatioLable
	{
		get
		{
			return _MicARatioLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicARatioLable = value;
		}
	}

	internal virtual Label MicADecTimeLable
	{
		get
		{
			return _MicADecTimeLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicADecTimeLable = value;
		}
	}

	internal virtual Label MicAAttTimeLable
	{
		get
		{
			return _MicAAttTimeLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicAAttTimeLable = value;
		}
	}

	internal virtual Label MicAThresholdLable
	{
		get
		{
			return _MicAThresholdLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicAThresholdLable = value;
		}
	}

	internal virtual Label Label269
	{
		get
		{
			return _Label269;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label269 = value;
		}
	}

	internal virtual Label Label352
	{
		get
		{
			return _Label352;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label352 = value;
		}
	}

	internal virtual TrackBar MicARatio
	{
		get
		{
			return _MicARatio;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicARatio = value;
		}
	}

	internal virtual TrackBar MicADecTime
	{
		get
		{
			return _MicADecTime;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicADecTime = value;
		}
	}

	internal virtual Label Label353
	{
		get
		{
			return _Label353;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label353 = value;
		}
	}

	internal virtual TrackBar MicAAttTime
	{
		get
		{
			return _MicAAttTime;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicAAttTime = value;
		}
	}

	internal virtual TrackBar MicAThreshold
	{
		get
		{
			return _MicAThreshold;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicAThreshold = value;
		}
	}

	internal virtual Label Label354
	{
		get
		{
			return _Label354;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label354 = value;
		}
	}

	internal virtual Label Label355
	{
		get
		{
			return _Label355;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label355 = value;
		}
	}

	internal virtual ComboBox MicA_FBE
	{
		get
		{
			return _MicA_FBE;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			EventHandler eventHandler = MicA_FBE_SelectedIndexChanged;
			if (_MicA_FBE != null)
			{
				_MicA_FBE.SelectedIndexChanged -= eventHandler;
			}
			_MicA_FBE = value;
			if (_MicA_FBE != null)
			{
				_MicA_FBE.SelectedIndexChanged += eventHandler;
			}
		}
	}

	internal virtual Label MicAHpfLable
	{
		get
		{
			return _MicAHpfLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicAHpfLable = value;
		}
	}

	internal virtual Label Label357
	{
		get
		{
			return _Label357;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label357 = value;
		}
	}

	internal virtual TrackBar MicAHpf
	{
		get
		{
			return _MicAHpf;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicAHpf = value;
		}
	}

	internal virtual Label Label358
	{
		get
		{
			return _Label358;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label358 = value;
		}
	}

	internal virtual Label Label180
	{
		get
		{
			return _Label180;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label180 = value;
		}
	}

	internal virtual Label Label182
	{
		get
		{
			return _Label182;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label182 = value;
		}
	}

	internal virtual Label Label183
	{
		get
		{
			return _Label183;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label183 = value;
		}
	}

	internal virtual Label Label192
	{
		get
		{
			return _Label192;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label192 = value;
		}
	}

	internal virtual PanelEx PanelEx1
	{
		get
		{
			return _PanelEx1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_PanelEx1 = value;
		}
	}

	internal virtual DomainUpDown MicQEQ16
	{
		get
		{
			return _MicQEQ16;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicQEQ16 = value;
		}
	}

	internal virtual Panel Panel27
	{
		get
		{
			return _Panel27;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel27 = value;
		}
	}

	internal virtual DomainUpDown MicQEQ17
	{
		get
		{
			return _MicQEQ17;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicQEQ17 = value;
		}
	}

	internal virtual DomainUpDown MicQEQ19
	{
		get
		{
			return _MicQEQ19;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicQEQ19 = value;
		}
	}

	internal virtual Panel Panel12
	{
		get
		{
			return _Panel12;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel12 = value;
		}
	}

	internal virtual ButtonX MicHS10
	{
		get
		{
			return _MicHS10;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicHS10 = value;
		}
	}

	internal virtual ButtonX MicLS10
	{
		get
		{
			return _MicLS10;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicLS10 = value;
		}
	}

	internal virtual ButtonX MicP10
	{
		get
		{
			return _MicP10;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicP10 = value;
		}
	}

	internal virtual Label Label156
	{
		get
		{
			return _Label156;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label156 = value;
		}
	}

	internal virtual Label Label161
	{
		get
		{
			return _Label161;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label161 = value;
		}
	}

	internal virtual Label Label162
	{
		get
		{
			return _Label162;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label162 = value;
		}
	}

	internal virtual DomainUpDown MicFreqEQ10
	{
		get
		{
			return _MicFreqEQ10;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicFreqEQ10 = value;
		}
	}

	internal virtual DomainUpDown MicGainEQ10
	{
		get
		{
			return _MicGainEQ10;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicGainEQ10 = value;
		}
	}

	internal virtual DomainUpDown MicQEQ10
	{
		get
		{
			return _MicQEQ10;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicQEQ10 = value;
		}
	}

	internal virtual TrackBar MicTrackBar10
	{
		get
		{
			return _MicTrackBar10;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicTrackBar10 = value;
		}
	}

	internal virtual Panel Panel13
	{
		get
		{
			return _Panel13;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel13 = value;
		}
	}

	internal virtual ButtonX MicHS5
	{
		get
		{
			return _MicHS5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicHS5 = value;
		}
	}

	internal virtual ButtonX MicLS5
	{
		get
		{
			return _MicLS5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicLS5 = value;
		}
	}

	internal virtual ButtonX MicP5
	{
		get
		{
			return _MicP5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicP5 = value;
		}
	}

	internal virtual Label Label184
	{
		get
		{
			return _Label184;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label184 = value;
		}
	}

	internal virtual Label Label185
	{
		get
		{
			return _Label185;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label185 = value;
		}
	}

	internal virtual Label Label187
	{
		get
		{
			return _Label187;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label187 = value;
		}
	}

	internal virtual DomainUpDown MicFreqEQ5
	{
		get
		{
			return _MicFreqEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicFreqEQ5 = value;
		}
	}

	internal virtual DomainUpDown MicGainEQ5
	{
		get
		{
			return _MicGainEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicGainEQ5 = value;
		}
	}

	internal virtual DomainUpDown MicQEQ5
	{
		get
		{
			return _MicQEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicQEQ5 = value;
		}
	}

	internal virtual TrackBar MicTrackBar5
	{
		get
		{
			return _MicTrackBar5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicTrackBar5 = value;
		}
	}

	internal virtual Panel Panel14
	{
		get
		{
			return _Panel14;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel14 = value;
		}
	}

	internal virtual ButtonX MicHS2
	{
		get
		{
			return _MicHS2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicHS2 = value;
		}
	}

	internal virtual ButtonX MicLS2
	{
		get
		{
			return _MicLS2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicLS2 = value;
		}
	}

	internal virtual ButtonX MicP2
	{
		get
		{
			return _MicP2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicP2 = value;
		}
	}

	internal virtual Label Label194
	{
		get
		{
			return _Label194;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label194 = value;
		}
	}

	internal virtual Label Label205
	{
		get
		{
			return _Label205;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label205 = value;
		}
	}

	internal virtual Label Label206
	{
		get
		{
			return _Label206;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label206 = value;
		}
	}

	internal virtual DomainUpDown MicFreqEQ2
	{
		get
		{
			return _MicFreqEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicFreqEQ2 = value;
		}
	}

	internal virtual DomainUpDown MicGainEQ2
	{
		get
		{
			return _MicGainEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicGainEQ2 = value;
		}
	}

	internal virtual DomainUpDown MicQEQ2
	{
		get
		{
			return _MicQEQ2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicQEQ2 = value;
		}
	}

	internal virtual TrackBar MicTrackBar2
	{
		get
		{
			return _MicTrackBar2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicTrackBar2 = value;
		}
	}

	internal virtual Panel Panel15
	{
		get
		{
			return _Panel15;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel15 = value;
		}
	}

	internal virtual ButtonX MicHS8
	{
		get
		{
			return _MicHS8;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicHS8 = value;
		}
	}

	internal virtual ButtonX MicLS8
	{
		get
		{
			return _MicLS8;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicLS8 = value;
		}
	}

	internal virtual ButtonX MicP8
	{
		get
		{
			return _MicP8;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicP8 = value;
		}
	}

	internal virtual Label Label208
	{
		get
		{
			return _Label208;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label208 = value;
		}
	}

	internal virtual Label Label209
	{
		get
		{
			return _Label209;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label209 = value;
		}
	}

	internal virtual Label Label210
	{
		get
		{
			return _Label210;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label210 = value;
		}
	}

	internal virtual DomainUpDown MicFreqEQ8
	{
		get
		{
			return _MicFreqEQ8;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicFreqEQ8 = value;
		}
	}

	internal virtual DomainUpDown MicGainEQ8
	{
		get
		{
			return _MicGainEQ8;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicGainEQ8 = value;
		}
	}

	internal virtual DomainUpDown MicQEQ8
	{
		get
		{
			return _MicQEQ8;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicQEQ8 = value;
		}
	}

	internal virtual TrackBar MicTrackBar8
	{
		get
		{
			return _MicTrackBar8;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicTrackBar8 = value;
		}
	}

	internal virtual Panel Panel16
	{
		get
		{
			return _Panel16;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel16 = value;
		}
	}

	internal virtual ButtonX MicHS6
	{
		get
		{
			return _MicHS6;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicHS6 = value;
		}
	}

	internal virtual ButtonX MicLS6
	{
		get
		{
			return _MicLS6;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicLS6 = value;
		}
	}

	internal virtual ButtonX MicP6
	{
		get
		{
			return _MicP6;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicP6 = value;
		}
	}

	internal virtual Label Label211
	{
		get
		{
			return _Label211;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label211 = value;
		}
	}

	internal virtual Label Label212
	{
		get
		{
			return _Label212;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label212 = value;
		}
	}

	internal virtual Label Label213
	{
		get
		{
			return _Label213;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label213 = value;
		}
	}

	internal virtual DomainUpDown MicFreqEQ6
	{
		get
		{
			return _MicFreqEQ6;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicFreqEQ6 = value;
		}
	}

	internal virtual DomainUpDown MicGainEQ6
	{
		get
		{
			return _MicGainEQ6;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicGainEQ6 = value;
		}
	}

	internal virtual DomainUpDown MicQEQ6
	{
		get
		{
			return _MicQEQ6;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicQEQ6 = value;
		}
	}

	internal virtual TrackBar MicTrackBar6
	{
		get
		{
			return _MicTrackBar6;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicTrackBar6 = value;
		}
	}

	internal virtual Panel Panel17
	{
		get
		{
			return _Panel17;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel17 = value;
		}
	}

	internal virtual ButtonX MicHS1
	{
		get
		{
			return _MicHS1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicHS1 = value;
		}
	}

	internal virtual ButtonX MicLS1
	{
		get
		{
			return _MicLS1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicLS1 = value;
		}
	}

	internal virtual ButtonX MicP1
	{
		get
		{
			return _MicP1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicP1 = value;
		}
	}

	internal virtual Label Label214
	{
		get
		{
			return _Label214;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label214 = value;
		}
	}

	internal virtual Label Label215
	{
		get
		{
			return _Label215;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label215 = value;
		}
	}

	internal virtual Label Label216
	{
		get
		{
			return _Label216;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label216 = value;
		}
	}

	internal virtual DomainUpDown MicFreqEQ1
	{
		get
		{
			return _MicFreqEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicFreqEQ1 = value;
		}
	}

	internal virtual DomainUpDown MicGainEQ1
	{
		get
		{
			return _MicGainEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicGainEQ1 = value;
		}
	}

	internal virtual DomainUpDown MicQEQ1
	{
		get
		{
			return _MicQEQ1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicQEQ1 = value;
		}
	}

	internal virtual TrackBar MicTrackBar1
	{
		get
		{
			return _MicTrackBar1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicTrackBar1 = value;
		}
	}

	internal virtual Panel Panel18
	{
		get
		{
			return _Panel18;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel18 = value;
		}
	}

	internal virtual ButtonX MicHS3
	{
		get
		{
			return _MicHS3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicHS3 = value;
		}
	}

	internal virtual ButtonX MicLS3
	{
		get
		{
			return _MicLS3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicLS3 = value;
		}
	}

	internal virtual ButtonX MicP3
	{
		get
		{
			return _MicP3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicP3 = value;
		}
	}

	internal virtual Label Label217
	{
		get
		{
			return _Label217;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label217 = value;
		}
	}

	internal virtual Label Label218
	{
		get
		{
			return _Label218;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label218 = value;
		}
	}

	internal virtual Label Label219
	{
		get
		{
			return _Label219;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label219 = value;
		}
	}

	internal virtual DomainUpDown MicFreqEQ3
	{
		get
		{
			return _MicFreqEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicFreqEQ3 = value;
		}
	}

	internal virtual DomainUpDown MicGainEQ3
	{
		get
		{
			return _MicGainEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicGainEQ3 = value;
		}
	}

	internal virtual DomainUpDown MicQEQ3
	{
		get
		{
			return _MicQEQ3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicQEQ3 = value;
		}
	}

	internal virtual TrackBar MicTrackBar3
	{
		get
		{
			return _MicTrackBar3;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicTrackBar3 = value;
		}
	}

	internal virtual Panel Panel19
	{
		get
		{
			return _Panel19;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel19 = value;
		}
	}

	internal virtual ButtonX MicHS7
	{
		get
		{
			return _MicHS7;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicHS7 = value;
		}
	}

	internal virtual ButtonX MicLS7
	{
		get
		{
			return _MicLS7;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicLS7 = value;
		}
	}

	internal virtual ButtonX MicP7
	{
		get
		{
			return _MicP7;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicP7 = value;
		}
	}

	internal virtual Label Label220
	{
		get
		{
			return _Label220;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label220 = value;
		}
	}

	internal virtual Label Label221
	{
		get
		{
			return _Label221;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label221 = value;
		}
	}

	internal virtual Label Label222
	{
		get
		{
			return _Label222;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label222 = value;
		}
	}

	internal virtual DomainUpDown MicFreqEQ7
	{
		get
		{
			return _MicFreqEQ7;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicFreqEQ7 = value;
		}
	}

	internal virtual DomainUpDown MicGainEQ7
	{
		get
		{
			return _MicGainEQ7;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicGainEQ7 = value;
		}
	}

	internal virtual DomainUpDown MicQEQ7
	{
		get
		{
			return _MicQEQ7;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicQEQ7 = value;
		}
	}

	internal virtual TrackBar MicTrackBar7
	{
		get
		{
			return _MicTrackBar7;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicTrackBar7 = value;
		}
	}

	internal virtual Panel Panel20
	{
		get
		{
			return _Panel20;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel20 = value;
		}
	}

	internal virtual ButtonX MicHS9
	{
		get
		{
			return _MicHS9;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicHS9 = value;
		}
	}

	internal virtual ButtonX MicLS9
	{
		get
		{
			return _MicLS9;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicLS9 = value;
		}
	}

	internal virtual ButtonX MicP9
	{
		get
		{
			return _MicP9;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicP9 = value;
		}
	}

	internal virtual Label Label223
	{
		get
		{
			return _Label223;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label223 = value;
		}
	}

	internal virtual Label Label224
	{
		get
		{
			return _Label224;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label224 = value;
		}
	}

	internal virtual Label Label225
	{
		get
		{
			return _Label225;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label225 = value;
		}
	}

	internal virtual DomainUpDown MicFreqEQ9
	{
		get
		{
			return _MicFreqEQ9;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicFreqEQ9 = value;
		}
	}

	internal virtual DomainUpDown MicGainEQ9
	{
		get
		{
			return _MicGainEQ9;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicGainEQ9 = value;
		}
	}

	internal virtual DomainUpDown MicQEQ9
	{
		get
		{
			return _MicQEQ9;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicQEQ9 = value;
		}
	}

	internal virtual TrackBar MicTrackBar9
	{
		get
		{
			return _MicTrackBar9;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicTrackBar9 = value;
		}
	}

	internal virtual Panel Panel21
	{
		get
		{
			return _Panel21;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel21 = value;
		}
	}

	internal virtual ButtonX MicHS4
	{
		get
		{
			return _MicHS4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicHS4 = value;
		}
	}

	internal virtual ButtonX MicLS4
	{
		get
		{
			return _MicLS4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicLS4 = value;
		}
	}

	internal virtual ButtonX MicP4
	{
		get
		{
			return _MicP4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicP4 = value;
		}
	}

	internal virtual Label Label226
	{
		get
		{
			return _Label226;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label226 = value;
		}
	}

	internal virtual Label Label227
	{
		get
		{
			return _Label227;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label227 = value;
		}
	}

	internal virtual Label Label228
	{
		get
		{
			return _Label228;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label228 = value;
		}
	}

	internal virtual DomainUpDown MicFreqEQ4
	{
		get
		{
			return _MicFreqEQ4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicFreqEQ4 = value;
		}
	}

	internal virtual DomainUpDown MicGainEQ4
	{
		get
		{
			return _MicGainEQ4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicGainEQ4 = value;
		}
	}

	internal virtual DomainUpDown MicQEQ4
	{
		get
		{
			return _MicQEQ4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicQEQ4 = value;
		}
	}

	internal virtual TrackBar MicTrackBar4
	{
		get
		{
			return _MicTrackBar4;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicTrackBar4 = value;
		}
	}

	internal virtual SuperTabControlPanel SuperTabControlPanel
	{
		get
		{
			return _SuperTabControlPanel;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SuperTabControlPanel = value;
		}
	}

	internal virtual SuperTabItem 输入
	{
		get
		{
			return _输入;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_输入 = value;
		}
	}

	internal virtual Panel Panel22
	{
		get
		{
			return _Panel22;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel22 = value;
		}
	}

	internal virtual ButtonX MicHS20
	{
		get
		{
			return _MicHS20;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicHS20 = value;
		}
	}

	internal virtual ButtonX MicLS20
	{
		get
		{
			return _MicLS20;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicLS20 = value;
		}
	}

	internal virtual ButtonX MicP20
	{
		get
		{
			return _MicP20;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicP20 = value;
		}
	}

	internal virtual Label Label229
	{
		get
		{
			return _Label229;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label229 = value;
		}
	}

	internal virtual Label Label230
	{
		get
		{
			return _Label230;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label230 = value;
		}
	}

	internal virtual Label Label231
	{
		get
		{
			return _Label231;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label231 = value;
		}
	}

	internal virtual DomainUpDown MicFreqEQ20
	{
		get
		{
			return _MicFreqEQ20;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicFreqEQ20 = value;
		}
	}

	internal virtual DomainUpDown MicGainEQ20
	{
		get
		{
			return _MicGainEQ20;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicGainEQ20 = value;
		}
	}

	internal virtual DomainUpDown MicQEQ20
	{
		get
		{
			return _MicQEQ20;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicQEQ20 = value;
		}
	}

	internal virtual TrackBar MicTrackBar20
	{
		get
		{
			return _MicTrackBar20;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicTrackBar20 = value;
		}
	}

	internal virtual Panel Panel23
	{
		get
		{
			return _Panel23;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel23 = value;
		}
	}

	internal virtual ButtonX MicHS15
	{
		get
		{
			return _MicHS15;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicHS15 = value;
		}
	}

	internal virtual ButtonX MicLS15
	{
		get
		{
			return _MicLS15;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicLS15 = value;
		}
	}

	internal virtual ButtonX MicP15
	{
		get
		{
			return _MicP15;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicP15 = value;
		}
	}

	internal virtual Label Label232
	{
		get
		{
			return _Label232;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label232 = value;
		}
	}

	internal virtual Label Label233
	{
		get
		{
			return _Label233;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label233 = value;
		}
	}

	internal virtual Label Label234
	{
		get
		{
			return _Label234;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label234 = value;
		}
	}

	internal virtual DomainUpDown MicFreqEQ15
	{
		get
		{
			return _MicFreqEQ15;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicFreqEQ15 = value;
		}
	}

	internal virtual DomainUpDown MicGainEQ15
	{
		get
		{
			return _MicGainEQ15;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicGainEQ15 = value;
		}
	}

	internal virtual DomainUpDown MicQEQ15
	{
		get
		{
			return _MicQEQ15;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicQEQ15 = value;
		}
	}

	internal virtual TrackBar MicTrackBar15
	{
		get
		{
			return _MicTrackBar15;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicTrackBar15 = value;
		}
	}

	internal virtual Panel Panel24
	{
		get
		{
			return _Panel24;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel24 = value;
		}
	}

	internal virtual ButtonX MicHS12
	{
		get
		{
			return _MicHS12;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicHS12 = value;
		}
	}

	internal virtual ButtonX MicLS12
	{
		get
		{
			return _MicLS12;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicLS12 = value;
		}
	}

	internal virtual ButtonX MicP12
	{
		get
		{
			return _MicP12;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicP12 = value;
		}
	}

	internal virtual Label Label235
	{
		get
		{
			return _Label235;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label235 = value;
		}
	}

	internal virtual Label Label236
	{
		get
		{
			return _Label236;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label236 = value;
		}
	}

	internal virtual Label Label237
	{
		get
		{
			return _Label237;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label237 = value;
		}
	}

	internal virtual DomainUpDown MicFreqEQ12
	{
		get
		{
			return _MicFreqEQ12;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicFreqEQ12 = value;
		}
	}

	internal virtual DomainUpDown MicGainEQ12
	{
		get
		{
			return _MicGainEQ12;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicGainEQ12 = value;
		}
	}

	internal virtual DomainUpDown MicQEQ12
	{
		get
		{
			return _MicQEQ12;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicQEQ12 = value;
		}
	}

	internal virtual TrackBar MicTrackBar12
	{
		get
		{
			return _MicTrackBar12;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicTrackBar12 = value;
		}
	}

	internal virtual Panel Panel25
	{
		get
		{
			return _Panel25;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel25 = value;
		}
	}

	internal virtual ButtonX MicHS18
	{
		get
		{
			return _MicHS18;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicHS18 = value;
		}
	}

	internal virtual ButtonX MicLS18
	{
		get
		{
			return _MicLS18;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicLS18 = value;
		}
	}

	internal virtual ButtonX MicP18
	{
		get
		{
			return _MicP18;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicP18 = value;
		}
	}

	internal virtual Label Label238
	{
		get
		{
			return _Label238;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label238 = value;
		}
	}

	internal virtual Label Label239
	{
		get
		{
			return _Label239;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label239 = value;
		}
	}

	internal virtual Label Label240
	{
		get
		{
			return _Label240;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label240 = value;
		}
	}

	internal virtual DomainUpDown MicFreqEQ18
	{
		get
		{
			return _MicFreqEQ18;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicFreqEQ18 = value;
		}
	}

	internal virtual DomainUpDown MicGainEQ18
	{
		get
		{
			return _MicGainEQ18;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicGainEQ18 = value;
		}
	}

	internal virtual DomainUpDown MicQEQ18
	{
		get
		{
			return _MicQEQ18;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicQEQ18 = value;
		}
	}

	internal virtual TrackBar MicTrackBar18
	{
		get
		{
			return _MicTrackBar18;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicTrackBar18 = value;
		}
	}

	internal virtual Panel Panel26
	{
		get
		{
			return _Panel26;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel26 = value;
		}
	}

	internal virtual ButtonX MicHS16
	{
		get
		{
			return _MicHS16;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicHS16 = value;
		}
	}

	internal virtual ButtonX MicLS16
	{
		get
		{
			return _MicLS16;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicLS16 = value;
		}
	}

	internal virtual ButtonX MicP16
	{
		get
		{
			return _MicP16;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicP16 = value;
		}
	}

	internal virtual Label Label241
	{
		get
		{
			return _Label241;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label241 = value;
		}
	}

	internal virtual Label Label242
	{
		get
		{
			return _Label242;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label242 = value;
		}
	}

	internal virtual Label Label243
	{
		get
		{
			return _Label243;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label243 = value;
		}
	}

	internal virtual DomainUpDown MicFreqEQ16
	{
		get
		{
			return _MicFreqEQ16;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicFreqEQ16 = value;
		}
	}

	internal virtual DomainUpDown MicGainEQ16
	{
		get
		{
			return _MicGainEQ16;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicGainEQ16 = value;
		}
	}

	internal virtual TrackBar MicTrackBar16
	{
		get
		{
			return _MicTrackBar16;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicTrackBar16 = value;
		}
	}

	internal virtual ButtonX MicHS11
	{
		get
		{
			return _MicHS11;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicHS11 = value;
		}
	}

	internal virtual ButtonX MicLS11
	{
		get
		{
			return _MicLS11;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicLS11 = value;
		}
	}

	internal virtual ButtonX MicP11
	{
		get
		{
			return _MicP11;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicP11 = value;
		}
	}

	internal virtual Label Label244
	{
		get
		{
			return _Label244;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label244 = value;
		}
	}

	internal virtual Label Label245
	{
		get
		{
			return _Label245;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label245 = value;
		}
	}

	internal virtual Label Label246
	{
		get
		{
			return _Label246;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label246 = value;
		}
	}

	internal virtual DomainUpDown MicFreqEQ11
	{
		get
		{
			return _MicFreqEQ11;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicFreqEQ11 = value;
		}
	}

	internal virtual DomainUpDown MicQEQ11
	{
		get
		{
			return _MicQEQ11;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicQEQ11 = value;
		}
	}

	internal virtual TrackBar MicTrackBar11
	{
		get
		{
			return _MicTrackBar11;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicTrackBar11 = value;
		}
	}

	internal virtual Panel Panel28
	{
		get
		{
			return _Panel28;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel28 = value;
		}
	}

	internal virtual ButtonX MicHS13
	{
		get
		{
			return _MicHS13;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicHS13 = value;
		}
	}

	internal virtual ButtonX MicLS13
	{
		get
		{
			return _MicLS13;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicLS13 = value;
		}
	}

	internal virtual ButtonX MicP13
	{
		get
		{
			return _MicP13;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicP13 = value;
		}
	}

	internal virtual Label Label247
	{
		get
		{
			return _Label247;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label247 = value;
		}
	}

	internal virtual Label Label248
	{
		get
		{
			return _Label248;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label248 = value;
		}
	}

	internal virtual Label Label249
	{
		get
		{
			return _Label249;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label249 = value;
		}
	}

	internal virtual DomainUpDown MicFreqEQ13
	{
		get
		{
			return _MicFreqEQ13;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicFreqEQ13 = value;
		}
	}

	internal virtual DomainUpDown MicGainEQ13
	{
		get
		{
			return _MicGainEQ13;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicGainEQ13 = value;
		}
	}

	internal virtual DomainUpDown MicQEQ13
	{
		get
		{
			return _MicQEQ13;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicQEQ13 = value;
		}
	}

	internal virtual TrackBar MicTrackBar13
	{
		get
		{
			return _MicTrackBar13;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicTrackBar13 = value;
		}
	}

	internal virtual Panel Panel29
	{
		get
		{
			return _Panel29;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel29 = value;
		}
	}

	internal virtual ButtonX MicHS17
	{
		get
		{
			return _MicHS17;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicHS17 = value;
		}
	}

	internal virtual ButtonX MicLS17
	{
		get
		{
			return _MicLS17;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicLS17 = value;
		}
	}

	internal virtual ButtonX MicP17
	{
		get
		{
			return _MicP17;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicP17 = value;
		}
	}

	internal virtual Label Label250
	{
		get
		{
			return _Label250;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label250 = value;
		}
	}

	internal virtual Label Label251
	{
		get
		{
			return _Label251;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label251 = value;
		}
	}

	internal virtual Label Label252
	{
		get
		{
			return _Label252;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label252 = value;
		}
	}

	internal virtual DomainUpDown MicFreqEQ17
	{
		get
		{
			return _MicFreqEQ17;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicFreqEQ17 = value;
		}
	}

	internal virtual DomainUpDown MicGainEQ17
	{
		get
		{
			return _MicGainEQ17;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicGainEQ17 = value;
		}
	}

	internal virtual Panel Panel30
	{
		get
		{
			return _Panel30;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel30 = value;
		}
	}

	internal virtual ButtonX MicHS19
	{
		get
		{
			return _MicHS19;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicHS19 = value;
		}
	}

	internal virtual ButtonX MicLS19
	{
		get
		{
			return _MicLS19;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicLS19 = value;
		}
	}

	internal virtual ButtonX MicP19
	{
		get
		{
			return _MicP19;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicP19 = value;
		}
	}

	internal virtual Label Label253
	{
		get
		{
			return _Label253;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label253 = value;
		}
	}

	internal virtual Label Label254
	{
		get
		{
			return _Label254;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label254 = value;
		}
	}

	internal virtual Label Label255
	{
		get
		{
			return _Label255;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label255 = value;
		}
	}

	internal virtual DomainUpDown MicFreqEQ19
	{
		get
		{
			return _MicFreqEQ19;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicFreqEQ19 = value;
		}
	}

	internal virtual DomainUpDown MicGainEQ19
	{
		get
		{
			return _MicGainEQ19;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicGainEQ19 = value;
		}
	}

	internal virtual TrackBar MicTrackBar19
	{
		get
		{
			return _MicTrackBar19;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicTrackBar19 = value;
		}
	}

	internal virtual Panel Panel31
	{
		get
		{
			return _Panel31;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel31 = value;
		}
	}

	internal virtual ButtonX MicHS14
	{
		get
		{
			return _MicHS14;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicHS14 = value;
		}
	}

	internal virtual ButtonX MicLS14
	{
		get
		{
			return _MicLS14;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicLS14 = value;
		}
	}

	internal virtual ButtonX MicP14
	{
		get
		{
			return _MicP14;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicP14 = value;
		}
	}

	internal virtual Label Label256
	{
		get
		{
			return _Label256;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label256 = value;
		}
	}

	internal virtual Label Label257
	{
		get
		{
			return _Label257;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label257 = value;
		}
	}

	internal virtual Label Label258
	{
		get
		{
			return _Label258;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label258 = value;
		}
	}

	internal virtual DomainUpDown MicFreqEQ14
	{
		get
		{
			return _MicFreqEQ14;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicFreqEQ14 = value;
		}
	}

	internal virtual DomainUpDown MicGainEQ14
	{
		get
		{
			return _MicGainEQ14;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicGainEQ14 = value;
		}
	}

	internal virtual DomainUpDown MicQEQ14
	{
		get
		{
			return _MicQEQ14;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicQEQ14 = value;
		}
	}

	internal virtual TrackBar MicTrackBar14
	{
		get
		{
			return _MicTrackBar14;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicTrackBar14 = value;
		}
	}

	internal virtual ButtonX Mic2HS20
	{
		get
		{
			return _Mic2HS20;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2HS20 = value;
		}
	}

	internal virtual ButtonX Mic2LS20
	{
		get
		{
			return _Mic2LS20;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2LS20 = value;
		}
	}

	internal virtual ButtonX Mic2P20
	{
		get
		{
			return _Mic2P20;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2P20 = value;
		}
	}

	internal virtual Label Label285
	{
		get
		{
			return _Label285;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label285 = value;
		}
	}

	internal virtual Label Label286
	{
		get
		{
			return _Label286;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label286 = value;
		}
	}

	internal virtual Label Label287
	{
		get
		{
			return _Label287;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label287 = value;
		}
	}

	internal virtual DomainUpDown Mic2FreqEQ20
	{
		get
		{
			return _Mic2FreqEQ20;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2FreqEQ20 = value;
		}
	}

	internal virtual DomainUpDown Mic2GainEQ20
	{
		get
		{
			return _Mic2GainEQ20;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2GainEQ20 = value;
		}
	}

	internal virtual DomainUpDown Mic2QEQ20
	{
		get
		{
			return _Mic2QEQ20;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2QEQ20 = value;
		}
	}

	internal virtual TrackBar Mic2TrackBar20
	{
		get
		{
			return _Mic2TrackBar20;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2TrackBar20 = value;
		}
	}

	internal virtual Panel Panel33
	{
		get
		{
			return _Panel33;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel33 = value;
		}
	}

	internal virtual ButtonX Mic2HS15
	{
		get
		{
			return _Mic2HS15;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2HS15 = value;
		}
	}

	internal virtual ButtonX Mic2LS15
	{
		get
		{
			return _Mic2LS15;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2LS15 = value;
		}
	}

	internal virtual ButtonX Mic2P15
	{
		get
		{
			return _Mic2P15;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2P15 = value;
		}
	}

	internal virtual Label Label288
	{
		get
		{
			return _Label288;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label288 = value;
		}
	}

	internal virtual Label Label289
	{
		get
		{
			return _Label289;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label289 = value;
		}
	}

	internal virtual Label Label290
	{
		get
		{
			return _Label290;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label290 = value;
		}
	}

	internal virtual DomainUpDown Mic2FreqEQ15
	{
		get
		{
			return _Mic2FreqEQ15;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2FreqEQ15 = value;
		}
	}

	internal virtual DomainUpDown Mic2GainEQ15
	{
		get
		{
			return _Mic2GainEQ15;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2GainEQ15 = value;
		}
	}

	internal virtual DomainUpDown Mic2QEQ15
	{
		get
		{
			return _Mic2QEQ15;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2QEQ15 = value;
		}
	}

	internal virtual ButtonX Mic2HS12
	{
		get
		{
			return _Mic2HS12;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2HS12 = value;
		}
	}

	internal virtual ButtonX Mic2LS12
	{
		get
		{
			return _Mic2LS12;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2LS12 = value;
		}
	}

	internal virtual ButtonX Mic2P12
	{
		get
		{
			return _Mic2P12;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2P12 = value;
		}
	}

	internal virtual Label Label291
	{
		get
		{
			return _Label291;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label291 = value;
		}
	}

	internal virtual Label Label292
	{
		get
		{
			return _Label292;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label292 = value;
		}
	}

	internal virtual Label Label293
	{
		get
		{
			return _Label293;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label293 = value;
		}
	}

	internal virtual DomainUpDown Mic2FreqEQ12
	{
		get
		{
			return _Mic2FreqEQ12;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2FreqEQ12 = value;
		}
	}

	internal virtual DomainUpDown Mic2GainEQ12
	{
		get
		{
			return _Mic2GainEQ12;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2GainEQ12 = value;
		}
	}

	internal virtual DomainUpDown Mic2QEQ12
	{
		get
		{
			return _Mic2QEQ12;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2QEQ12 = value;
		}
	}

	internal virtual Panel Panel35
	{
		get
		{
			return _Panel35;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel35 = value;
		}
	}

	internal virtual ButtonX Mic2HS18
	{
		get
		{
			return _Mic2HS18;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2HS18 = value;
		}
	}

	internal virtual ButtonX Mic2LS18
	{
		get
		{
			return _Mic2LS18;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2LS18 = value;
		}
	}

	internal virtual ButtonX Mic2P18
	{
		get
		{
			return _Mic2P18;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2P18 = value;
		}
	}

	internal virtual Label Label294
	{
		get
		{
			return _Label294;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label294 = value;
		}
	}

	internal virtual Label Label295
	{
		get
		{
			return _Label295;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label295 = value;
		}
	}

	internal virtual Label Label296
	{
		get
		{
			return _Label296;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label296 = value;
		}
	}

	internal virtual DomainUpDown Mic2FreqEQ18
	{
		get
		{
			return _Mic2FreqEQ18;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2FreqEQ18 = value;
		}
	}

	internal virtual DomainUpDown Mic2GainEQ18
	{
		get
		{
			return _Mic2GainEQ18;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2GainEQ18 = value;
		}
	}

	internal virtual DomainUpDown Mic2QEQ18
	{
		get
		{
			return _Mic2QEQ18;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2QEQ18 = value;
		}
	}

	internal virtual TrackBar Mic2TrackBar18
	{
		get
		{
			return _Mic2TrackBar18;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2TrackBar18 = value;
		}
	}

	internal virtual Panel Panel36
	{
		get
		{
			return _Panel36;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel36 = value;
		}
	}

	internal virtual ButtonX Mic2HS16
	{
		get
		{
			return _Mic2HS16;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2HS16 = value;
		}
	}

	internal virtual ButtonX Mic2LS16
	{
		get
		{
			return _Mic2LS16;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2LS16 = value;
		}
	}

	internal virtual ButtonX Mic2P16
	{
		get
		{
			return _Mic2P16;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2P16 = value;
		}
	}

	internal virtual Label Label297
	{
		get
		{
			return _Label297;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label297 = value;
		}
	}

	internal virtual Label Label298
	{
		get
		{
			return _Label298;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label298 = value;
		}
	}

	internal virtual Label Label299
	{
		get
		{
			return _Label299;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label299 = value;
		}
	}

	internal virtual DomainUpDown Mic2FreqEQ16
	{
		get
		{
			return _Mic2FreqEQ16;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2FreqEQ16 = value;
		}
	}

	internal virtual DomainUpDown Mic2GainEQ16
	{
		get
		{
			return _Mic2GainEQ16;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2GainEQ16 = value;
		}
	}

	internal virtual DomainUpDown Mic2QEQ16
	{
		get
		{
			return _Mic2QEQ16;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2QEQ16 = value;
		}
	}

	internal virtual TrackBar Mic2TrackBar16
	{
		get
		{
			return _Mic2TrackBar16;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2TrackBar16 = value;
		}
	}

	internal virtual ButtonX Mic2HS11
	{
		get
		{
			return _Mic2HS11;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2HS11 = value;
		}
	}

	internal virtual ButtonX Mic2LS11
	{
		get
		{
			return _Mic2LS11;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2LS11 = value;
		}
	}

	internal virtual ButtonX Mic2P11
	{
		get
		{
			return _Mic2P11;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2P11 = value;
		}
	}

	internal virtual Label Label300
	{
		get
		{
			return _Label300;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label300 = value;
		}
	}

	internal virtual Label Label301
	{
		get
		{
			return _Label301;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label301 = value;
		}
	}

	internal virtual Label Label302
	{
		get
		{
			return _Label302;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label302 = value;
		}
	}

	internal virtual DomainUpDown Mic2FreqEQ11
	{
		get
		{
			return _Mic2FreqEQ11;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2FreqEQ11 = value;
		}
	}

	internal virtual DomainUpDown Mic2GainEQ11
	{
		get
		{
			return _Mic2GainEQ11;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2GainEQ11 = value;
		}
	}

	internal virtual DomainUpDown Mic2QEQ11
	{
		get
		{
			return _Mic2QEQ11;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2QEQ11 = value;
		}
	}

	internal virtual Panel Panel38
	{
		get
		{
			return _Panel38;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel38 = value;
		}
	}

	internal virtual ButtonX Mic2HS13
	{
		get
		{
			return _Mic2HS13;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2HS13 = value;
		}
	}

	internal virtual ButtonX Mic2LS13
	{
		get
		{
			return _Mic2LS13;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2LS13 = value;
		}
	}

	internal virtual ButtonX Mic2P13
	{
		get
		{
			return _Mic2P13;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2P13 = value;
		}
	}

	internal virtual Label Label303
	{
		get
		{
			return _Label303;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label303 = value;
		}
	}

	internal virtual Label Label304
	{
		get
		{
			return _Label304;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label304 = value;
		}
	}

	internal virtual Label Label305
	{
		get
		{
			return _Label305;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label305 = value;
		}
	}

	internal virtual DomainUpDown Mic2FreqEQ13
	{
		get
		{
			return _Mic2FreqEQ13;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2FreqEQ13 = value;
		}
	}

	internal virtual DomainUpDown Mic2GainEQ13
	{
		get
		{
			return _Mic2GainEQ13;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2GainEQ13 = value;
		}
	}

	internal virtual DomainUpDown Mic2QEQ13
	{
		get
		{
			return _Mic2QEQ13;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2QEQ13 = value;
		}
	}

	internal virtual TrackBar Mic2TrackBar13
	{
		get
		{
			return _Mic2TrackBar13;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2TrackBar13 = value;
		}
	}

	internal virtual Panel Panel39
	{
		get
		{
			return _Panel39;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel39 = value;
		}
	}

	internal virtual ButtonX Mic2HS17
	{
		get
		{
			return _Mic2HS17;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2HS17 = value;
		}
	}

	internal virtual ButtonX Mic2LS17
	{
		get
		{
			return _Mic2LS17;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2LS17 = value;
		}
	}

	internal virtual ButtonX Mic2P17
	{
		get
		{
			return _Mic2P17;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2P17 = value;
		}
	}

	internal virtual Label Label306
	{
		get
		{
			return _Label306;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label306 = value;
		}
	}

	internal virtual Label Label307
	{
		get
		{
			return _Label307;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label307 = value;
		}
	}

	internal virtual Label Label308
	{
		get
		{
			return _Label308;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label308 = value;
		}
	}

	internal virtual DomainUpDown Mic2FreqEQ17
	{
		get
		{
			return _Mic2FreqEQ17;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2FreqEQ17 = value;
		}
	}

	internal virtual DomainUpDown Mic2GainEQ17
	{
		get
		{
			return _Mic2GainEQ17;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2GainEQ17 = value;
		}
	}

	internal virtual DomainUpDown Mic2QEQ17
	{
		get
		{
			return _Mic2QEQ17;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2QEQ17 = value;
		}
	}

	internal virtual TrackBar Mic2TrackBar17
	{
		get
		{
			return _Mic2TrackBar17;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2TrackBar17 = value;
		}
	}

	internal virtual Panel Panel40
	{
		get
		{
			return _Panel40;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel40 = value;
		}
	}

	internal virtual ButtonX Mic2HS19
	{
		get
		{
			return _Mic2HS19;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2HS19 = value;
		}
	}

	internal virtual ButtonX Mic2LS19
	{
		get
		{
			return _Mic2LS19;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2LS19 = value;
		}
	}

	internal virtual ButtonX Mic2P19
	{
		get
		{
			return _Mic2P19;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2P19 = value;
		}
	}

	internal virtual Label Label309
	{
		get
		{
			return _Label309;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label309 = value;
		}
	}

	internal virtual Label Label310
	{
		get
		{
			return _Label310;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label310 = value;
		}
	}

	internal virtual Label Label311
	{
		get
		{
			return _Label311;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label311 = value;
		}
	}

	internal virtual DomainUpDown Mic2FreqEQ19
	{
		get
		{
			return _Mic2FreqEQ19;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2FreqEQ19 = value;
		}
	}

	internal virtual DomainUpDown Mic2GainEQ19
	{
		get
		{
			return _Mic2GainEQ19;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2GainEQ19 = value;
		}
	}

	internal virtual DomainUpDown Mic2QEQ19
	{
		get
		{
			return _Mic2QEQ19;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2QEQ19 = value;
		}
	}

	internal virtual TrackBar Mic2TrackBar19
	{
		get
		{
			return _Mic2TrackBar19;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2TrackBar19 = value;
		}
	}

	internal virtual Panel Panel41
	{
		get
		{
			return _Panel41;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel41 = value;
		}
	}

	internal virtual ButtonX Mic2HS14
	{
		get
		{
			return _Mic2HS14;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2HS14 = value;
		}
	}

	internal virtual ButtonX Mic2LS14
	{
		get
		{
			return _Mic2LS14;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2LS14 = value;
		}
	}

	internal virtual ButtonX Mic2P14
	{
		get
		{
			return _Mic2P14;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2P14 = value;
		}
	}

	internal virtual Label Label312
	{
		get
		{
			return _Label312;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label312 = value;
		}
	}

	internal virtual Label Label313
	{
		get
		{
			return _Label313;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label313 = value;
		}
	}

	internal virtual Label Label314
	{
		get
		{
			return _Label314;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label314 = value;
		}
	}

	internal virtual DomainUpDown Mic2FreqEQ14
	{
		get
		{
			return _Mic2FreqEQ14;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2FreqEQ14 = value;
		}
	}

	internal virtual DomainUpDown Mic2GainEQ14
	{
		get
		{
			return _Mic2GainEQ14;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2GainEQ14 = value;
		}
	}

	internal virtual DomainUpDown Mic2QEQ14
	{
		get
		{
			return _Mic2QEQ14;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2QEQ14 = value;
		}
	}

	internal virtual DomainUpDown MicGainEQ11
	{
		get
		{
			return _MicGainEQ11;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicGainEQ11 = value;
		}
	}

	internal virtual Label EchoHighDampLable
	{
		get
		{
			return _EchoHighDampLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoHighDampLable = value;
		}
	}

	internal virtual Label Label5
	{
		get
		{
			return _Label5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label5 = value;
		}
	}

	internal virtual TrackBar EchoHighDamp
	{
		get
		{
			return _EchoHighDamp;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EchoHighDamp = value;
		}
	}

	internal virtual SuperTabControlPanel SuperTabControlPanel11
	{
		get
		{
			return _SuperTabControlPanel11;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SuperTabControlPanel11 = value;
		}
	}

	internal virtual GroupBox GroupBox2
	{
		get
		{
			return _GroupBox2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_GroupBox2 = value;
		}
	}

	internal virtual Label RecRevVolPhase
	{
		get
		{
			return _RecRevVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RecRevVolPhase = value;
		}
	}

	internal virtual TrackBar RecRevVol
	{
		get
		{
			return _RecRevVol;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RecRevVol = value;
		}
	}

	internal virtual Label RecRevVolLable
	{
		get
		{
			return _RecRevVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RecRevVolLable = value;
		}
	}

	internal virtual Label RecEchoVolPhase
	{
		get
		{
			return _RecEchoVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RecEchoVolPhase = value;
		}
	}

	internal virtual Label RecEchoVolLable
	{
		get
		{
			return _RecEchoVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RecEchoVolLable = value;
		}
	}

	internal virtual Label RecMicVolPhase
	{
		get
		{
			return _RecMicVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RecMicVolPhase = value;
		}
	}

	internal virtual Label RecMicVolLable
	{
		get
		{
			return _RecMicVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RecMicVolLable = value;
		}
	}

	internal virtual Label RecMusicVolPhase
	{
		get
		{
			return _RecMusicVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RecMusicVolPhase = value;
		}
	}

	internal virtual Label RecMusicVolLable
	{
		get
		{
			return _RecMusicVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RecMusicVolLable = value;
		}
	}

	internal virtual Label Label18
	{
		get
		{
			return _Label18;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label18 = value;
		}
	}

	internal virtual Label Label19
	{
		get
		{
			return _Label19;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label19 = value;
		}
	}

	internal virtual Label Label20
	{
		get
		{
			return _Label20;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label20 = value;
		}
	}

	internal virtual Label Label21
	{
		get
		{
			return _Label21;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label21 = value;
		}
	}

	internal virtual TrackBar RecEchoVol
	{
		get
		{
			return _RecEchoVol;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RecEchoVol = value;
		}
	}

	internal virtual TrackBar RecMicVol
	{
		get
		{
			return _RecMicVol;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RecMicVol = value;
		}
	}

	internal virtual TrackBar RecMusicVol
	{
		get
		{
			return _RecMusicVol;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RecMusicVol = value;
		}
	}

	internal virtual TrackBar MicTrackBar17
	{
		get
		{
			return _MicTrackBar17;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MicTrackBar17 = value;
		}
	}

	internal virtual TrackBar Mic2TrackBar15
	{
		get
		{
			return _Mic2TrackBar15;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2TrackBar15 = value;
		}
	}

	internal virtual TrackBar Mic2TrackBar12
	{
		get
		{
			return _Mic2TrackBar12;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2TrackBar12 = value;
		}
	}

	internal virtual TrackBar Mic2TrackBar11
	{
		get
		{
			return _Mic2TrackBar11;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2TrackBar11 = value;
		}
	}

	internal virtual TrackBar Mic2TrackBar14
	{
		get
		{
			return _Mic2TrackBar14;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Mic2TrackBar14 = value;
		}
	}

	internal virtual Panel Panel1
	{
		get
		{
			return _Panel1;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel1 = value;
		}
	}

	internal virtual Panel Panel86
	{
		get
		{
			return _Panel86;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel86 = value;
		}
	}

	internal virtual Panel Panel85
	{
		get
		{
			return _Panel85;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel85 = value;
		}
	}

	internal virtual Panel Panel87
	{
		get
		{
			return _Panel87;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel87 = value;
		}
	}

	internal virtual Panel Panel88
	{
		get
		{
			return _Panel88;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel88 = value;
		}
	}

	internal virtual Panel Panel91
	{
		get
		{
			return _Panel91;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel91 = value;
		}
	}

	internal virtual Panel Panel90
	{
		get
		{
			return _Panel90;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel90 = value;
		}
	}

	internal virtual Button MainMuteR
	{
		get
		{
			return _MainMuteR;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainMuteR = value;
		}
	}

	internal virtual Button MainMuteL
	{
		get
		{
			return _MainMuteL;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_MainMuteL = value;
		}
	}

	internal virtual Button SubMute
	{
		get
		{
			return _SubMute;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SubMute = value;
		}
	}

	internal virtual Button CenMute
	{
		get
		{
			return _CenMute;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_CenMute = value;
		}
	}

	internal virtual ButtonX EQBypassRev
	{
		get
		{
			return _EQBypassRev;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EQBypassRev = value;
		}
	}

	internal virtual ButtonX EQResetRev
	{
		get
		{
			return _EQResetRev;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EQResetRev = value;
		}
	}

	internal virtual ButtonX EQBypassMicA
	{
		get
		{
			return _EQBypassMicA;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EQBypassMicA = value;
		}
	}

	internal virtual ButtonX EQResetMicA
	{
		get
		{
			return _EQResetMicA;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EQResetMicA = value;
		}
	}

	internal virtual ButtonX EQBypassEcho
	{
		get
		{
			return _EQBypassEcho;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EQBypassEcho = value;
		}
	}

	internal virtual ButtonX EQResetEcho
	{
		get
		{
			return _EQResetEcho;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EQResetEcho = value;
		}
	}

	internal virtual ButtonX EQBypassMicB
	{
		get
		{
			return _EQBypassMicB;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EQBypassMicB = value;
		}
	}

	internal virtual ButtonX EQResetMicB
	{
		get
		{
			return _EQResetMicB;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EQResetMicB = value;
		}
	}

	internal virtual ButtonX EQBypassSub
	{
		get
		{
			return _EQBypassSub;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EQBypassSub = value;
		}
	}

	internal virtual ButtonX EQResetSub
	{
		get
		{
			return _EQResetSub;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EQResetSub = value;
		}
	}

	internal virtual ButtonX EQBypassCen
	{
		get
		{
			return _EQBypassCen;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EQBypassCen = value;
		}
	}

	internal virtual ButtonX EQResetCen
	{
		get
		{
			return _EQResetCen;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EQResetCen = value;
		}
	}

	internal virtual ButtonX EQBypassMain
	{
		get
		{
			return _EQBypassMain;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EQBypassMain = value;
		}
	}

	internal virtual ButtonX EQResetMain
	{
		get
		{
			return _EQResetMain;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EQResetMain = value;
		}
	}

	internal virtual SuperTabControlPanel SuperTabControlPanel12
	{
		get
		{
			return _SuperTabControlPanel12;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SuperTabControlPanel12 = value;
		}
	}

	internal virtual SuperTabControlPanel SuperTabControlPanel13
	{
		get
		{
			return _SuperTabControlPanel13;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_SuperTabControlPanel13 = value;
		}
	}

	internal virtual Panel Panel98
	{
		get
		{
			return _Panel98;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel98 = value;
		}
	}

	internal virtual Label Label67
	{
		get
		{
			return _Label67;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label67 = value;
		}
	}

	internal virtual Label Label87
	{
		get
		{
			return _Label87;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label87 = value;
		}
	}

	internal virtual Label Label88
	{
		get
		{
			return _Label88;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label88 = value;
		}
	}

	internal virtual Label Label89
	{
		get
		{
			return _Label89;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label89 = value;
		}
	}

	internal virtual Label Label93
	{
		get
		{
			return _Label93;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label93 = value;
		}
	}

	internal virtual Panel Panel92
	{
		get
		{
			return _Panel92;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel92 = value;
		}
	}

	internal virtual Label Label8
	{
		get
		{
			return _Label8;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label8 = value;
		}
	}

	internal virtual Label Label41
	{
		get
		{
			return _Label41;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label41 = value;
		}
	}

	internal virtual Label Label42
	{
		get
		{
			return _Label42;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label42 = value;
		}
	}

	internal virtual Label Label43
	{
		get
		{
			return _Label43;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label43 = value;
		}
	}

	internal virtual Label Label47
	{
		get
		{
			return _Label47;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label47 = value;
		}
	}

	internal virtual ButtonX EQBypassRevB
	{
		get
		{
			return _EQBypassRevB;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EQBypassRevB = value;
		}
	}

	internal virtual ButtonX EQResetRevB
	{
		get
		{
			return _EQResetRevB;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_EQResetRevB = value;
		}
	}

	internal virtual Label RevBTimeLable
	{
		get
		{
			return _RevBTimeLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBTimeLable = value;
		}
	}

	internal virtual Label RevBPreDelayLable
	{
		get
		{
			return _RevBPreDelayLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBPreDelayLable = value;
		}
	}

	internal virtual Label Label70
	{
		get
		{
			return _Label70;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label70 = value;
		}
	}

	internal virtual Label Label71
	{
		get
		{
			return _Label71;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label71 = value;
		}
	}

	internal virtual TrackBar RevBTime
	{
		get
		{
			return _RevBTime;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBTime = value;
		}
	}

	internal virtual TrackBar RevBPreDelay
	{
		get
		{
			return _RevBPreDelay;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBPreDelay = value;
		}
	}

	internal virtual Label RevBLpfLable
	{
		get
		{
			return _RevBLpfLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBLpfLable = value;
		}
	}

	internal virtual Label RevBHpfLable
	{
		get
		{
			return _RevBHpfLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBHpfLable = value;
		}
	}

	internal virtual Label Label74
	{
		get
		{
			return _Label74;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label74 = value;
		}
	}

	internal virtual TrackBar RevBLpf
	{
		get
		{
			return _RevBLpf;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBLpf = value;
		}
	}

	internal virtual Label Label77
	{
		get
		{
			return _Label77;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label77 = value;
		}
	}

	internal virtual TrackBar RevBHpf
	{
		get
		{
			return _RevBHpf;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBHpf = value;
		}
	}

	internal virtual Label RevBDirectVolPhase
	{
		get
		{
			return _RevBDirectVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBDirectVolPhase = value;
		}
	}

	internal virtual Label RevBDirectVolLable
	{
		get
		{
			return _RevBDirectVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBDirectVolLable = value;
		}
	}

	internal virtual Label RevBVolPhase
	{
		get
		{
			return _RevBVolPhase;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBVolPhase = value;
		}
	}

	internal virtual Label RevBVolLable
	{
		get
		{
			return _RevBVolLable;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBVolLable = value;
		}
	}

	internal virtual Label Label85
	{
		get
		{
			return _Label85;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label85 = value;
		}
	}

	internal virtual Label Label86
	{
		get
		{
			return _Label86;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label86 = value;
		}
	}

	internal virtual TrackBar RevBDirectVolTrabar
	{
		get
		{
			return _RevBDirectVolTrabar;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBDirectVolTrabar = value;
		}
	}

	internal virtual TrackBar RevBVolTrabar
	{
		get
		{
			return _RevBVolTrabar;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBVolTrabar = value;
		}
	}

	internal virtual Panel Panel99
	{
		get
		{
			return _Panel99;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel99 = value;
		}
	}

	internal virtual ButtonX RevBHS5
	{
		get
		{
			return _RevBHS5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBHS5 = value;
		}
	}

	internal virtual ButtonX RevBLS5
	{
		get
		{
			return _RevBLS5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBLS5 = value;
		}
	}

	internal virtual ButtonX RevBP5
	{
		get
		{
			return _RevBP5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBP5 = value;
		}
	}

	internal virtual Label Label90
	{
		get
		{
			return _Label90;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label90 = value;
		}
	}

	internal virtual Label Label91
	{
		get
		{
			return _Label91;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label91 = value;
		}
	}

	internal virtual Label Label92
	{
		get
		{
			return _Label92;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Label92 = value;
		}
	}

	internal virtual DomainUpDown RevBFreqEQ5
	{
		get
		{
			return _RevBFreqEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBFreqEQ5 = value;
		}
	}

	internal virtual DomainUpDown RevBGainEQ5
	{
		get
		{
			return _RevBGainEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBGainEQ5 = value;
		}
	}

	internal virtual DomainUpDown RevBQEQ5
	{
		get
		{
			return _RevBQEQ5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBQEQ5 = value;
		}
	}

	internal virtual TrackBar RevBTrackBar5
	{
		get
		{
			return _RevBTrackBar5;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBTrackBar5 = value;
		}
	}

	internal virtual Panel Panel100
	{
		get
		{
			return _Panel100;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_Panel100 = value;
		}
	}

	internal virtual ButtonX RevBHS2
	{
		get
		{
			return _RevBHS2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBHS2 = value;
		}
	}

	internal virtual ButtonX RevBLS2
	{
		get
		{
			return _RevBLS2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBLS2 = value;
		}
	}

	internal virtual ButtonX RevBP2
	{
		get
		{
			return _RevBP2;
		}
		[MethodImpl(MethodImplOptions.Synchronized)]
		set
		{
			_RevBP2 = value;
		}
	}
