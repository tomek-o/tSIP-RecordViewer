object frmSettings: TfrmSettings
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Settings'
  ClientHeight = 286
  ClientWidth = 459
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object pnlBottom: TPanel
    Left = 0
    Top = 249
    Width = 459
    Height = 37
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    DesignSize = (
      459
      37)
    object btnCancel: TButton
      Left = 375
      Top = 6
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Cancel'
      TabOrder = 1
      OnClick = btnCancelClick
    end
    object btnApply: TButton
      Left = 294
      Top = 6
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Apply'
      TabOrder = 0
      OnClick = btnApplyClick
    end
  end
  object pcGeneral: TPageControl
    Left = 0
    Top = 0
    Width = 459
    Height = 249
    ActivePage = tsWhisperCpp
    Align = alClient
    TabOrder = 0
    object tsGeneral: TTabSheet
      Caption = 'General'
      object chbAlwaysOnTop: TCheckBox
        Left = 3
        Top = 3
        Width = 325
        Height = 17
        Caption = 'Window always on top'
        TabOrder = 0
        OnClick = chbAlwaysOnTopClick
      end
    end
    object tsAudio: TTabSheet
      Caption = 'Audio'
      ImageIndex = 2
      object lblAudioOutput: TLabel
        Left = 16
        Top = 11
        Width = 34
        Height = 13
        Caption = 'Output'
      end
      object btnRefreshAudioDevicesLists: TButton
        Left = 16
        Top = 35
        Width = 185
        Height = 25
        Caption = 'Refresh audio devices lists'
        TabOrder = 0
      end
      object cbAudioOutput: TComboBox
        Left = 56
        Top = 8
        Width = 285
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 1
      end
    end
    object tsWhisperCpp: TTabSheet
      Caption = 'S2T: whisper.cpp'
      ImageIndex = 3
      object lblWhisperExe: TLabel
        Left = 3
        Top = 6
        Width = 169
        Height = 13
        Caption = 'whisper.cpp executable (main.exe)'
      end
      object lblWhisperModel: TLabel
        Left = 3
        Top = 30
        Width = 118
        Height = 13
        Caption = 'Model file (ggml-XXX.bin)'
      end
      object lblWhisperLanguage: TLabel
        Left = 3
        Top = 54
        Width = 160
        Height = 13
        Caption = 'Language code (optional, e.g. pl)'
      end
      object lblWhisperThreadCount: TLabel
        Left = 3
        Top = 78
        Width = 64
        Height = 13
        Caption = 'Thread count'
      end
      object lblPathsInfo: TLabel
        Left = 3
        Top = 120
        Width = 254
        Height = 13
        Caption = 'Note: application tries to treat paths as relative first.'
      end
      object edWhisperExe: TEdit
        Left = 178
        Top = 3
        Width = 239
        Height = 21
        TabOrder = 0
      end
      object btnSelectWhisperExe: TButton
        Left = 418
        Top = 3
        Width = 25
        Height = 21
        Caption = '...'
        TabOrder = 1
        OnClick = btnSelectWhisperExeClick
      end
      object edWhisperModel: TEdit
        Left = 178
        Top = 27
        Width = 239
        Height = 21
        TabOrder = 2
      end
      object btnSelectWhisperModel: TButton
        Left = 418
        Top = 27
        Width = 25
        Height = 21
        Caption = '...'
        TabOrder = 3
        OnClick = btnSelectWhisperModelClick
      end
      object edWhisperLanguage: TEdit
        Left = 178
        Top = 51
        Width = 71
        Height = 21
        TabOrder = 4
      end
      object cbWhisperThreadCount: TComboBox
        Left = 178
        Top = 75
        Width = 57
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        ItemIndex = 2
        TabOrder = 5
        Text = '3'
        Items.Strings = (
          '1'
          '2'
          '3'
          '4'
          '5'
          '6'
          '7'
          '8'
          '9'
          '10'
          '11'
          '12'
          '13'
          '14'
          '15'
          '16'
          '17'
          '18'
          '19'
          '20'
          '21'
          '22'
          '23'
          '24'
          '25'
          '26'
          '27'
          '28'
          '29'
          '30'
          '31'
          '32')
      end
    end
    object pcLogging: TTabSheet
      Caption = 'Logging'
      ImageIndex = 1
      object lblUiCapacity: TLabel
        Left = 5
        Top = 26
        Width = 194
        Height = 13
        Caption = 'Visible (buffered) log lines in log window:'
      end
      object cmbMaxUiLogLines: TComboBox
        Left = 216
        Top = 23
        Width = 111
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 0
        OnChange = cmbMaxUiLogLinesChange
        Items.Strings = (
          '100'
          '200'
          '500'
          '1000'
          '2000'
          '5000'
          '10000')
      end
      object chbLogToFile: TCheckBox
        Left = 5
        Top = 3
        Width = 325
        Height = 17
        Caption = 'Log to file'
        TabOrder = 1
        OnClick = chbAlwaysOnTopClick
      end
    end
  end
  object openDialog: TOpenDialog
    Left = 8
    Top = 176
  end
end
