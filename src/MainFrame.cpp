// MainFrame.cpp
#include "MainFrame.h"
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/artprov.h>

void MainFrame::BindEvents()
{
    // m_addReplaceBtn->Bind(wxEVT_BUTTON, &MainFrame::OnAddReplace, this);
    // m_renameBtn->Bind(wxEVT_BUTTON, &MainFrame::OnRename, this);
    // 移除原来的Bind(wxEVT_DROP_FILES...)
}

void MainFrame::OnAddReplace(wxCommandEvent& event)
{
    // TODO: Implement adding another replace rule
    wxMessageBox("添加替换规则功能待实现", "提示", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnRename(wxCommandEvent& event)
{
    if (m_files.IsEmpty()) {
        wxMessageBox("没有可重命名的文件", "错误", wxOK | wxICON_ERROR);
        return;
    }
    int method = m_notebook ? m_notebook->GetSelection() : 0;
    wxString findStr = m_findText ? m_findText->GetValue() : "";
    wxString replaceStr = m_replaceText ? m_replaceText->GetValue() : "";
    bool allowExtChange = m_allowExtChange ? m_allowExtChange->GetValue() : false;
    if (method == 0 && findStr.IsEmpty() && replaceStr.IsEmpty()) {
        wxMessageBox("请输入查找和替换内容", "错误", wxOK | wxICON_ERROR);
        return;
    }
    for (size_t i = 0; i < m_files.GetCount(); i++) {
        wxFileName file(m_files[i]);
        wxString newName;
        switch (method) {
            case 1: // 追加
                newName = file.GetName() + replaceStr;
                break;
            case 2: // 自动序号
                newName = wxString::Format("%s_%d", file.GetName(), i+1);
                break;
            default: // 替换
                newName = file.GetName();
                newName.Replace(findStr, replaceStr);
        }
        if (allowExtChange) {
            wxString ext = file.GetExt();
            ext.Replace(findStr, replaceStr);
            newName = newName + "." + ext;
        } else {
            newName = newName + "." + file.GetExt();
        }
        wxString newPath = file.GetPathWithSep() + newName;
        if (wxRenameFile(m_files[i], newPath)) {
            m_files[i] = newPath;
        } else {
            wxLogError("无法重命名文件: %s", m_files[i]);
        }
    }
    // Update preview list
    UpdateFileList();
    wxMessageBox("重命名完成", "完成", wxOK | wxICON_INFORMATION);
}

void MainFrame::UpdateFileList()
{
    m_dropPreviewList->DeleteAllItems();
    if (m_files.IsEmpty()) {
        m_simpleBook->SetSelection(0);
        SetStatusText("当前文件数：0");
        m_mainPanel->Layout();
        return;
    } else {
        m_simpleBook->SetSelection(1);
        m_mainPanel->Layout();
    }
    int method = m_notebook ? m_notebook->GetSelection() : 0;
    wxString findStr = m_findText ? m_findText->GetValue() : "";
    wxString replaceStr = m_replaceText ? m_replaceText->GetValue() : "";
    bool allowExtChange = m_allowExtChange ? m_allowExtChange->GetValue() : false;
    for (size_t i = 0; i < m_files.GetCount(); ++i) {
        const wxString& filePath = m_files[i];
        wxFileName file(filePath);
        wxString previewName;
        if (method == 0 && findStr.IsEmpty() && replaceStr.IsEmpty()) {
            // 查找和替换都为空，预览名与原名一致
            previewName = file.GetFullName();
        } else {
            switch (method) {
                case 1: // 追加
                    previewName = file.GetName() + replaceStr;
                    break;
                case 2: // 自动序号
                    previewName = wxString::Format("%s_%d", file.GetName(), i+1);
                    break;
                default: // 替换
                    previewName = file.GetName();
                    previewName.Replace(findStr, replaceStr);
            }
            if (allowExtChange) {
                wxString ext = file.GetExt();
                ext.Replace(findStr, replaceStr);
                previewName = previewName + "." + ext;
            } else {
                previewName = previewName + "." + file.GetExt();
            }
        }
        int itemIndex = m_dropPreviewList->InsertItem(i, file.GetFullName());
        m_dropPreviewList->SetItem(itemIndex, 1, previewName); // 展示预览名
    }
    m_dropPreviewList->Refresh();
    SetStatusText(wxString::Format("当前文件数：%lu", m_files.GetCount()));
    if (wxStaticText* statusText = dynamic_cast<wxStaticText*>(FindWindowById(wxID_ANY, m_mainPanel))) {
        statusText->SetLabel(wxString::Format("排序 - 按名称升序 | 共 %d 个", m_files.GetCount()));
    }
}


// MainFrame 实现
MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600))
{
    // 添加菜单栏
    wxMenuBar* menuBar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(10001, "选择文件");
    fileMenu->Append(10002, "选择文件夹");
    menuBar->Append(fileMenu, "文件");
    SetMenuBar(menuBar);

    // 创建状态栏
    CreateStatusBar();
    SetStatusText(wxString::Format("当前文件数：%lu", m_files.GetCount()));

    CreateControls();
    BindEvents();
    SetDropTarget(new FileDropTarget(this));
    SetMinSize(wxSize(600, 400));
    Centre();

    // 绑定菜单事件
    Bind(wxEVT_MENU, &MainFrame::OnSelectFile, this, 10001);
    Bind(wxEVT_MENU, &MainFrame::OnSelectFolder, this, 10002);
}

void MainFrame::CreateControls()
{
    m_mainPanel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    // 创建选项卡区
    m_notebook = new wxNotebook(m_mainPanel, wxID_ANY);
    wxPanel* replacePanel = new wxPanel(m_notebook);
    CreateReplaceTab(replacePanel);
    m_notebook->AddPage(replacePanel, "替换", true);
    wxPanel* appendPanel = new wxPanel(m_notebook);
    CreateAppendTab(appendPanel);
    m_notebook->AddPage(appendPanel, "追加");
    wxPanel* autoNumPanel = new wxPanel(m_notebook);
    CreateAutoNumberTab(autoNumPanel);
    m_notebook->AddPage(autoNumPanel, "自动序号");
    mainSizer->Add(m_notebook, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5); // 选项卡区
    // 创建按钮区
    wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* clearBtn = new wxButton(m_mainPanel, wxID_ANY, "清空列表", wxDefaultPosition, wxSize(110, -1));
    clearBtn->SetBitmap(wxArtProvider::GetBitmap(wxART_DELETE, wxART_BUTTON, wxSize(20,20)));
    clearBtn->SetBitmapMargins(wxSize(4,0));
    clearBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        m_files.Clear();
        UpdateFileList();
    });
    btnSizer->Add(clearBtn, 0, wxEXPAND | wxRIGHT, 10);
    wxButton* delBtn = new wxButton(m_mainPanel, wxID_ANY, "删除选中行", wxDefaultPosition, wxSize(110, -1));
    delBtn->SetBitmap(wxArtProvider::GetBitmap(wxART_CUT, wxART_BUTTON, wxSize(20,20)));
    delBtn->SetBitmapMargins(wxSize(4,0));
    delBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        long sel = m_dropPreviewList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (sel != -1) {
            m_files.RemoveAt(sel);
            UpdateFileList();
        }
    });
    btnSizer->Add(delBtn, 0, wxEXPAND | wxRIGHT, 10);
    wxButton* renameBtn = new wxButton(m_mainPanel, wxID_ANY, "确定重命名", wxDefaultPosition, wxSize(110, -1));
    renameBtn->SetBitmap(wxArtProvider::GetBitmap(wxART_TICK_MARK, wxART_BUTTON, wxSize(20,20)));
    renameBtn->SetBitmapMargins(wxSize(4,0));
    renameBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt) {
        OnRename(evt);
    });
    btnSizer->Add(renameBtn, 0, wxEXPAND | wxRIGHT, 10);
    wxButton* previewBtn = new wxButton(m_mainPanel, wxID_ANY, "预览", wxDefaultPosition, wxSize(110, -1));
    previewBtn->SetBitmap(wxArtProvider::GetBitmap(wxART_FIND, wxART_BUTTON, wxSize(20,20)));
    previewBtn->SetBitmapMargins(wxSize(4,0));
    previewBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        UpdateFileList();
    });
    btnSizer->Add(previewBtn, 0, wxEXPAND);
    mainSizer->Add(btnSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP | wxBOTTOM, 10); // 按钮区
    // 文件列表区
    wxStaticBoxSizer* dropSizer = new wxStaticBoxSizer(wxVERTICAL, m_mainPanel, "文件列表");
    m_simpleBook = new wxSimplebook(dropSizer->GetStaticBox(), wxID_ANY);
    wxPanel* hintPanel = new wxPanel(m_simpleBook, wxID_ANY);
    wxBoxSizer* hintSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* iconTextVBox = new wxBoxSizer(wxVERTICAL);
    wxStaticBitmap* dropIcon = new wxStaticBitmap(hintPanel, wxID_ANY, wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_OTHER, wxSize(32,32)));
    iconTextVBox->Add(dropIcon, 0, wxALIGN_CENTER | wxBOTTOM, 8);
    m_dropHint = new wxStaticText(hintPanel, wxID_ANY, "拖放 / 粘贴文件(夹)到这", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
    m_dropHint->SetMinSize(wxSize(-1, 32));
    iconTextVBox->Add(m_dropHint, 0, wxALIGN_CENTER);
    hintSizer->AddStretchSpacer(1);
    hintSizer->Add(iconTextVBox, 0, wxALIGN_CENTER | wxALL, 10);
    hintSizer->AddStretchSpacer(1);
    hintPanel->SetSizer(hintSizer);
    hintPanel->SetDropTarget(new FileDropTarget(this));
    m_dropPreviewList = new wxListCtrl(m_simpleBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    m_dropPreviewList->InsertColumn(0, "原始文件名", wxLIST_FORMAT_LEFT, 250);
    m_dropPreviewList->InsertColumn(1, "修改后文件名预览", wxLIST_FORMAT_LEFT, 250);
    m_dropPreviewList->SetDropTarget(new FileDropTarget(this));
    m_simpleBook->AddPage(hintPanel, "提示", true);
    m_simpleBook->AddPage(m_dropPreviewList, "文件列表", false);
    dropSizer->Add(m_simpleBook, 1, wxEXPAND);
    mainSizer->Add(dropSizer, 2, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5); // 文件列表区
    m_mainPanel->SetSizer(mainSizer);
}

FileDropTarget::FileDropTarget(MainFrame* frame)
    : m_frame(frame)
{
    // 设置为可以接受拖放文件
    SetDataObject(new wxFileDataObject());
}

bool FileDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
    
    if (m_frame)
    {
        for (const wxString& filename : filenames)
        {
            m_frame->AddDroppedFile(filename);
        }
        m_frame->UpdateFileList();
        return true;
    }
    return false;
}
void MainFrame::AddDroppedFile(const wxString& filename)
{
    m_files.Add(filename);
}
void MainFrame::CreateReplaceTab(wxWindow* parent)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* findLabel = new wxStaticText(parent, wxID_ANY, "查找：");
    m_findText = new wxTextCtrl(parent, wxID_ANY, "", wxDefaultPosition, wxSize(-1, -1), 0, wxDefaultValidator, "find_text");
    sizer->Add(findLabel, 0, wxLEFT | wxRIGHT | wxTOP, 6);
    sizer->Add(m_findText, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 6);
    wxStaticText* replaceLabel = new wxStaticText(parent, wxID_ANY, "替换为：");
    m_replaceText = new wxTextCtrl(parent, wxID_ANY, "", wxDefaultPosition, wxSize(-1, -1), 0, wxDefaultValidator, "replace_text");
    sizer->Add(replaceLabel, 0, wxLEFT | wxRIGHT | wxTOP, 6);
    sizer->Add(m_replaceText, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 6);
    m_allowExtChange = new wxCheckBox(parent, wxID_ANY, "允许替换文件扩展名");
    sizer->Add(m_allowExtChange, 0, wxLEFT | wxRIGHT | wxBOTTOM, 6);
    parent->SetSizer(sizer);
}

void MainFrame::CreateAppendTab(wxWindow* parent)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(parent, wxID_ANY, "追加文本:"), 0, wxLEFT | wxRIGHT | wxTOP, 6);
    m_appendText = new wxTextCtrl(parent, wxID_ANY);
    sizer->Add(m_appendText, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 6);
    m_atStart = new wxCheckBox(parent, wxID_ANY, "在文件名开头追加");
    sizer->Add(m_atStart, 0, wxLEFT | wxRIGHT | wxBOTTOM, 6);
    parent->SetSizer(sizer);
}

void MainFrame::CreateAutoNumberTab(wxWindow* parent)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 2, 5, 5); // 间距缩小
    gridSizer->Add(new wxStaticText(parent, wxID_ANY, "起始序号："), 0, wxLEFT | wxRIGHT | wxTOP, 6);
    gridSizer->Add(new wxStaticText(parent, wxID_ANY, "增量："), 0, wxLEFT | wxRIGHT | wxTOP, 6);
    m_startNum = new wxSpinCtrl(parent, wxID_ANY, "1", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1000, 1);
    m_increment = new wxSpinCtrl(parent, wxID_ANY, "1", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100, 1);
    gridSizer->Add(m_startNum, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 6);
    gridSizer->Add(m_increment, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 6);
    sizer->Add(gridSizer, 0, wxEXPAND | wxALL, 0);
    m_formatRadio = new wxRadioBox(parent, wxID_ANY, "序号格式", wxDefaultPosition, wxDefaultSize,
                                   wxArrayString(3, new const wxChar*[]{
        wxT("1, 2, 3..."), wxT("01, 02, 03..."), wxT("001, 002, 003...")
    }), 1, wxRA_SPECIFY_COLS);
    sizer->Add(m_formatRadio, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 6);
    parent->SetSizer(sizer);
}

// 菜单事件处理
void MainFrame::OnSelectFile(wxCommandEvent& event)
{
    wxFileDialog dlg(this, "选择文件", wxEmptyString, wxEmptyString, "所有文件 (*.*)|*.*", wxFD_OPEN | wxFD_MULTIPLE);
    if (dlg.ShowModal() == wxID_OK) {
        wxArrayString paths;
        dlg.GetPaths(paths);
        for (const auto& path : paths) {
            m_files.Add(path);
        }
        UpdateFileList();
    }
}

void MainFrame::OnSelectFolder(wxCommandEvent& event)
{
    wxDirDialog dlg(this, "选择文件夹");
    if (dlg.ShowModal() == wxID_OK) {
        wxString dirPath = dlg.GetPath();
        wxArrayString files;
        wxDir::GetAllFiles(dirPath, &files);
        for (const auto& file : files) {
            m_files.Add(file);
        }
        UpdateFileList();
    }
}