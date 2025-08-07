// MainFrame.h
#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/filectrl.h>
#include <wx/dnd.h>
#include <wx/spinctrl.h>  // 添加wxSpinCtrl头文件
#include <wx/radiobox.h>  // 添加wxRadioBox头文件
#include <wx/dir.h> // 添加wxDir头文件
#include <wx/sizer.h> // 添加wxStackedSizer头文件
#include <wx/simplebook.h> // 替换 wxStackedSizer 头文件

class MainFrame;

class FileDropTarget : public wxFileDropTarget
{
public:
    explicit FileDropTarget(MainFrame* frame);
    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) override;

private:
    MainFrame* m_frame;
};

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title);
    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
    void OnAddReplace(wxCommandEvent& event); // 添加该声明
    void OnRename(wxCommandEvent& event);     // 添加该声明
    void AddDroppedFile(const wxString& filename); // 新增声明
    void OnSelectFile(wxCommandEvent& event); // 菜单事件声明
    void OnSelectFolder(wxCommandEvent& event); // 菜单事件声明
    void UpdateFileList();

private:
    void CreateControls();
    void CreateReplaceTab(wxWindow* parent);
    void CreateAppendTab(wxWindow* parent);
    void CreateAutoNumberTab(wxWindow* parent);
    void BindEvents();

    wxListCtrl* m_dropPreviewList; // 拖放文件预览列表
    
    // 主控件
    wxPanel* m_mainPanel;
    wxNotebook* m_notebook;
    wxStaticText* m_dropHint;
    wxSimplebook* m_simpleBook; // 用于切换提示和文件列表
    
    // 替换标签页控件
    wxTextCtrl* m_findText;
    wxTextCtrl* m_replaceText;
    wxCheckBox* m_allowExtChange;
    
    // 追加标签页控件
    wxTextCtrl* m_appendText;  // 追加文本输入框
    wxCheckBox* m_atStart;  // 是否在文件名开头追加复选框
    
    // 自动序号标签页控件
    wxSpinCtrl* m_startNum;  // 起始序号
    wxSpinCtrl* m_increment;  // 增量
    wxRadioBox* m_formatRadio;  // 序号格式
    
    wxArrayString m_files;
    // wxStackedSizer* m_stackedSizer; // 移除
};

#endif // MAINFRAME_H