VERSION 5.00
Begin VB.Form frmID3Test 
   Caption         =   "ID3 Test"
   ClientHeight    =   5625
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   5655
   LinkTopic       =   "Form1"
   ScaleHeight     =   5625
   ScaleWidth      =   5655
   StartUpPosition =   3  'Windows Default
   Begin VB.ListBox List1 
      Height          =   2595
      Left            =   120
      TabIndex        =   3
      Top             =   2880
      Width           =   5415
   End
   Begin VB.FileListBox File1 
      Height          =   2625
      Left            =   2400
      Pattern         =   "*.mp3;*.tag"
      TabIndex        =   2
      Top             =   120
      Width           =   3135
   End
   Begin VB.DirListBox Dir1 
      Height          =   2115
      Left            =   120
      TabIndex        =   1
      Top             =   600
      Width           =   2055
   End
   Begin VB.DriveListBox Drive1 
      Height          =   315
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   2055
   End
End
Attribute VB_Name = "frmID3Test"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub Dir1_Change()
    On Error Resume Next
    File1.Path = Dir1.Path
End Sub

Private Sub Drive1_Change()
    On Error Resume Next
    Dir1.Path = Drive1.Drive
End Sub

Private Sub File1_Click()
    Dim oTag As New ID3ComTag
    Dim oFrame As ID3ComFrame
    Dim oField As ID3ComField
    Dim n As Long
    
    List1.Clear
    
    oTag.Link File1.Path & "\" & File1.FileName
    
    For Each oFrame In oTag
        Debug.Print oFrame.FrameName
        Set oField = oFrame.Field(ID3_FIELD_TEXT)
        If Not oField Is Nothing Then
            For n = 1 To oField.NumTextItems
                List1.AddItem oFrame.FrameName & " - " & oField.Text(n)
            Next n
        End If
    Next
End Sub
