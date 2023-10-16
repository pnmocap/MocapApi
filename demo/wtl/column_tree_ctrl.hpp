#pragma once
#include <atlctrls.h>
#include <atlcrack.h>


inline BOOL extractSubString(WTL::CString& rString, LPCTSTR lpszFullString,
    int iSubString, TCHAR chSep)
{
    if (lpszFullString == NULL)
        return FALSE;

    while (iSubString--)
    {
        lpszFullString = _tcschr(lpszFullString, chSep);
        if (lpszFullString == NULL)
        {
            rString.Empty();        // return empty string as well
            return FALSE;
        }
        lpszFullString++;       // point past the separator
    }
    LPCTSTR lpchEnd = _tcschr(lpszFullString, chSep);
    int nLen = (lpchEnd == NULL) ?
        static_cast<int>(_tcslen(lpszFullString)) : (int)(lpchEnd - lpszFullString);
    ATLASSERT(nLen >= 0);
    Checked::memcpy_s(rString.GetBufferSetLength(nLen), nLen * sizeof(TCHAR),
        lpszFullString, nLen * sizeof(TCHAR));
    rString.ReleaseBuffer();	// Need to call ReleaseBuffer 
    // after calling GetBufferSetLength
    return TRUE;
}

template <class TBase>
class CColumnTreeCtrlT;

typedef struct _CTVHITTESTINFO {
    POINT pt;
    UINT flags;
    HTREEITEM hItem;
    int iSubItem;
} CTVHITTESTINFO;

template <class TBase>
class CCustomTreeChildCtrlT 
    : public CWindowImpl<CCustomTreeChildCtrlT<TBase>, CTreeViewCtrlT<TBase>>
{
    friend class CColumnTreeCtrlT<TBase>;
    using Super = CWindowImpl<CCustomTreeChildCtrlT<TBase>, CTreeViewCtrlT<TBase>>;
public:
    CCustomTreeChildCtrlT() 
    {}

    ~CCustomTreeChildCtrlT() 
    {}

    BOOL Create(_In_ DWORD dwStyle, _In_ RECT rect, _In_ HWND hParentWnd, _In_ UINT nID)
    {
        m_hWnd = Super::Create(hParentWnd, rect, NULL, dwStyle, 0, nID);
        return NULL != m_hWnd;
    }

    BOOL GetBkImage(LVBKIMAGE* plvbkImage) const 
    {
        ::memcpy(plvbkImage, &m_bkImage, sizeof(LVBKIMAGE));
        return TRUE;
    }

    BOOL SetBkImage(LVBKIMAGE* plvbkImage)
    {
        ::memcpy(&m_bkImage, plvbkImage, sizeof(LVBKIMAGE));
        Invalidate();
        return TRUE;
    }

    BEGIN_MSG_MAP(CCustomTreeChildCtrlT<TBase>)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_LBUTTONDBLCLK(OnLButtonDoubleClick)
        MSG_WM_KEYDOWN(OnKeyDown)
    END_MSG_MAP()
    
protected:
    int m_nFirstColumnWidth; // the width of the first column 
    int m_nOffsetX;      	 // offset of this window inside the parent 
    LVBKIMAGE m_bkImage;	 // information about background image
    CImageList m_imgBtns;	 // tree buttons images (IDB_TREEBTNS)

protected:
    BOOL CheckHit(CPoint point) 
    {
        // return TRUE if should pass the message to CTreeCtrl

        UINT fFlags;
        HTREEITEM hItem = HitTest(point, &fFlags);

        CRect rcItem, rcClient;
        GetClientRect(rcClient);
        GetItemRect(hItem, &rcItem, TRUE);

        if (fFlags & TVHT_ONITEMICON ||
            fFlags & TVHT_ONITEMBUTTON ||
            fFlags & TVHT_ONITEMSTATEICON)
            return TRUE;

        if (GetStyle() & TVS_FULLROWSELECT)
        {
            rcItem.right = rcClient.right;
            if (rcItem.PtInRect(point))
                return TRUE;

            return FALSE;
        }


        // verify the hit result
        if (fFlags & TVHT_ONITEMLABEL)
        {
            rcItem.right = m_nFirstColumnWidth;
            // check if within the first column
            if (!rcItem.PtInRect(point))
                return FALSE;

            WTL::CString strSub;
            WTL::CString itemText;
            GetItemText(hItem, itemText);
            extractSubString(strSub, itemText, 0, '\t');

            CDCHandle pDC = GetDC();
            pDC.SelectFont(GetFont());
            SIZE szTextExtent;
            pDC.GetTextExtent(strSub.GetBuffer(strSub.GetLength()), 
                strSub.GetLength(), &szTextExtent);
            rcItem.right = rcItem.left + szTextExtent.cx + 6;
            ReleaseDC(pDC);

            // check if inside the label's rectangle
            if (!rcItem.PtInRect(point))
                return FALSE;

            return TRUE;
        }

        return FALSE;
    }

protected:
    LRESULT OnPaint(_In_ UINT /*uMsg*/,
        _In_ WPARAM /*wParam*/,
        _In_ LPARAM /*lParam*/,
        _Out_ BOOL& bHandled) 
    {
        CRect rcClient;
        GetClientRect(&rcClient);

        CPaintDC dc(m_hWnd);

        CDC dcMem;
        CBitmap bmpMem;

        // use temporary bitmap to avoid flickering
        dcMem.CreateCompatibleDC(dc);
        if (bmpMem.CreateCompatibleBitmap(dc, rcClient.Width(), rcClient.Height()))
        {
            CBitmapHandle pOldBmp = dcMem.SelectBitmap(bmpMem);

            // paint the window onto the memory bitmap

#ifdef _OWNER_DRAWN_TREE	// if owner-drawn
            OwnerDraw(&dcMem);	// use our code
#else						// else use standard code
            Super::DefWindowProc(WM_PAINT, (WPARAM)dcMem.m_hDC, 0);
#endif //_OWNER_DRAWN_TREE

            // copy it to the window's DC
            dc.BitBlt(0, 0, rcClient.right, rcClient.bottom, dcMem, 0, 0, SRCCOPY);

            dcMem.SelectBitmap(pOldBmp);

            bmpMem.DeleteObject();
        }
        dcMem.DeleteDC();
        bHandled = TRUE;
        return 0;
    }

    LRESULT OnEraseBackground(_In_ UINT /*uMsg*/,
        _In_ WPARAM /*wParam*/,
        _In_ LPARAM /*lParam*/,
        _Out_ BOOL& bHandled) 
    {
        bHandled = TRUE;
        return 0;
    }

    void OnLButtonDown(UINT nFlags, CPoint point) 
    {
        // CTreeCtrl may scroll the bitmap up or down in several cases,
        // so we need to invalidate entire client area
        Invalidate();

        //... and pass to the base class
        //Super::OnLButtonDown( nFlags, point) 
        SetMsgHandled(FALSE);
    }

    void OnMouseMove(UINT nFlags, CPoint point) 
    {
        // mask mouse move if outside the real item's label
        if (CheckHit(point))
        {
            // call standard handler
            //Super::OnMouseMove(nFlags, point);
            SetMsgHandled(FALSE);
        }
    }

    void OnLButtonDoubleClick(UINT nFlags, CPoint point) 
    {
        // process this message only if double-clicked the real item's label
    // mask double click if outside the real item's label
        if (CheckHit(point))
        {
            HTREEITEM hItem = HitTest(point, NULL);
            if (hItem)
            {
#ifdef _OWNER_DRAWN_TREE
                // if the clicked item is partially visible we should invalidate
                // entire client area to avoid background bitmap scrolling
                CRect rcItem, rcClient;
                GetClientRect(&rcClient);
                GetItemRect(hItem, &rcItem, FALSE);
                if (rcItem.bottom > rcClient.bottom)
                {
                    Invalidate();
                    Super::OnLButtonDown(nFlags, point);
                    return;
                }
#endif //_OWNER_DRAWN_TREE

                SelectItem(hItem);
            }

            // call standard message handler
            //Super::OnLButtonDoubleClick(nFlags, point);
            SetMsgHandled(FALSE);
        }
        else
        {
            // if clicked outside the item's label
            // than set focus to contol window
            SetFocus();
        }
    }

    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
    {
        // CTreeCtrl may scroll the bitmap up or down in several cases,
        // so we need to invalidate entire client area
        Invalidate();

        //... and pass to the base class
        //Super::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
        SetMsgHandled(FALSE);
    }
};

template <class TBase>
class CColumnTreeCtrlT 
    : public CWindowImpl<CColumnTreeCtrlT<TBase>, CStaticT<TBase>>
{
    using Super = CWindowImpl<CColumnTreeCtrlT<TBase>, CStaticT<TBase>>;
public:

    using CCustomTreeChildCtrl = CCustomTreeChildCtrlT<TBase>;

    using CScrollBar = CScrollBarT<TBase>;

    using CHeaderCtrl = CHeaderCtrlT<TBase>;

    BEGIN_MSG_MAP(CColumnTreeCtrlT<TBase>)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
        MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
        MSG_WM_ENABLE(OnEnable)
        MSG_WM_SETTINGCHANGE(OnSettingChange)
        MSG_WM_SIZE(OnSize)
        MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
        MSG_WM_CANCELMODE(OnCancelMode)
    END_MSG_MAP()

    BOOL SubclassWindow(HWND hWnd) 
    {
        if (Super::SubclassWindow(hWnd)) {
            PostSubclassWindow();
            return TRUE;
        }
        else {
            return FALSE;
        }
    }

    BOOL Create(DWORD dwStyle, _In_ _U_RECT rect, HWND hParentWnd, UINT nID)
    {
        Super::Create(hParentWnd, rect, NULL, dwStyle, 0, nID);
        PostSubclassWindow();
        return NULL != m_hWnd;
    }

    int InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat = 0, int nWidth = -1, int nSubItem = -1) 
    {
        // update the header control in upper-right corner
    // to make it look the same way as main header

        if (nCol + 1 > m_arrColWidths.size()) {
            m_arrColWidths.resize(nCol + 1);
            m_arrColFormats.resize(nCol + 1);
        }

        CHeaderCtrl& header = GetHeaderCtrl();

        HDITEM hditem;
        hditem.mask = HDI_TEXT | HDI_WIDTH | HDI_FORMAT;
        hditem.fmt = nFormat;
        hditem.cxy = nWidth;
        hditem.pszText = (LPTSTR)lpszColumnHeading;
        m_arrColFormats[nCol] = nFormat;
        int indx = header.InsertItem(nCol, &hditem);

        if (m_Header.GetItemCount() > 0)
        {
            // if the main header contains items, 
            // insert an item to m_Header2
            hditem.pszText = _T("");
            hditem.cxy = GetSystemMetrics(SM_CXVSCROLL) + 5;
            m_Header2.InsertItem(0, &hditem);
        };
        UpdateColumns();

        return indx;
    }
    BOOL DeleteColumn(int nCol) 
    {
        // update the header control in upper-right corner
        // to make it look the same way as main header

        BOOL bResult = m_Header.DeleteItem(nCol);
        if (m_Header.GetItemCount() == 0)
        {
            m_Header2.DeleteItem(0);
        }

        UpdateColumns();
        return bResult;
    }

    CHeaderCtrl& GetHeaderCtrl() { return m_Header; }

    CCustomTreeChildCtrl& GetTreeCtrl() { return m_Tree; }

    void SetItemText(HTREEITEM hItem, int nColumn, LPCTSTR lpszItem)
    {
        WTL::CString szText;
        m_Tree.GetItemText(hItem, szText);
        WTL::CString szNewText, szSubItem;
        int i;
        for (i = 0; i < m_Header.GetItemCount(); i++)
        {
            extractSubString(szSubItem, szText, i, '\t');
            if (i != nColumn) szNewText += szSubItem + _T("\t");
            else szNewText += WTL::CString(lpszItem) + _T("\t");
        }
        m_Tree.SetItemText(hItem, szNewText);
    }

    void GetItemText(HTREEITEM hItem, int nColumn, WTL::CString& szSubItem)
    {
        // retreive and return the substring from tree item's text
        WTL::CString szText;
        m_Tree.GetItemText(hItem, szText);
        extractSubString(szSubItem, szText, nColumn, '\t');
    }

    // Call this function to determine the location of the specified point 
    // relative to the client area of a tree view control.
    HTREEITEM HitTest(CPoint pt, UINT* pFlags = NULL) const 
    {
        CTVHITTESTINFO htinfo = { pt, 0, NULL, 0 };
        HTREEITEM hItem = HitTest(&htinfo);
        if (pFlags)
        {
            *pFlags = htinfo.flags;
        }
        return hItem;
    }

    // Call this function to determine the location of the specified point 
    // relative to the client area of a tree view control.
    HTREEITEM HitTest(CTVHITTESTINFO* pHitTestInfo) const 
    {
        // We should use our own HitTest() method, because our custom tree
    // has different layout than the original CTreeCtrl.

        UINT uFlags = 0;
        HTREEITEM hItem = NULL;
        CRect rcItem, rcClient;

        CPoint point = pHitTestInfo->pt;
        point.x += m_xPos;
        point.y -= m_cyHeader;

        hItem = m_Tree.HitTest(point, &uFlags);

        // Fill the CTVHITTESTINFO structure
        pHitTestInfo->hItem = hItem;
        pHitTestInfo->flags = uFlags;
        pHitTestInfo->iSubItem = 0;

        if (!(uFlags & TVHT_ONITEMLABEL || uFlags & TVHT_ONITEMRIGHT))
            return hItem;

        // Additional check for item's label.
        // Determine correct subitem's index.

        int i;
        int x = 0;
        for (i = 0; i < m_Header.GetItemCount(); i++)
        {
            if (point.x >= x && point.x <= x + m_arrColWidths[i])
            {
                pHitTestInfo->iSubItem = i;
                pHitTestInfo->flags = TVHT_ONITEMLABEL;
                return hItem;
            }
            x += m_arrColWidths[i];
        }

        pHitTestInfo->hItem = NULL;
        pHitTestInfo->flags = TVHT_NOWHERE;
        return NULL;
    }

protected:
    virtual void PostSubclassWindow() 
    {
        if (m_Tree.m_hWnd)
            return; // do not initialize twice

        CRect rcClient;
        GetClientRect(&rcClient);

        // create tree and header controls as children
        m_Tree.Create(WS_CHILD | WS_VISIBLE | TVS_NOHSCROLL | TVS_NOTOOLTIPS, CRect(), m_hWnd, TreeID);
        m_Header.Create(m_hWnd, rcClient, NULL, WS_CHILD | HDS_BUTTONS | WS_VISIBLE | HDS_FULLDRAG, 0, HeaderID);
        m_Header2.Create(m_hWnd, rcClient, NULL, WS_CHILD, 0, Header2ID);

        // create horisontal scroll bar
        m_horScroll.Create(m_hWnd, rcClient, NULL, SBS_HORZ | WS_CHILD | SBS_BOTTOMALIGN, 0, HScrollID);

        // set correct font for the header
        CFontHandle font = m_Tree.GetFont();
        m_Header.SetFont(font);
        m_Header2.SetFont(font);


        // check if the common controls library version 6.0 is available
        BOOL bIsComCtl6 = FALSE;

        HMODULE hComCtlDll = LoadLibrary(_T("comctl32.dll"));

        if (hComCtlDll)
        {
            typedef HRESULT(CALLBACK* PFNDLLGETVERSION)(DLLVERSIONINFO*);

            PFNDLLGETVERSION pfnDllGetVersion = (PFNDLLGETVERSION)GetProcAddress(hComCtlDll, "DllGetVersion");

            if (pfnDllGetVersion)
            {
                DLLVERSIONINFO dvi;
                ZeroMemory(&dvi, sizeof(dvi));
                dvi.cbSize = sizeof(dvi);

                HRESULT hRes = (*pfnDllGetVersion)(&dvi);

                if (SUCCEEDED(hRes) && dvi.dwMajorVersion >= 6)
                    bIsComCtl6 = TRUE;
            }

            FreeLibrary(hComCtlDll);
        }
        // get header layout
        WINDOWPOS wp;
        HDLAYOUT hdlayout;
        hdlayout.prc = &rcClient;
        hdlayout.pwpos = &wp;
        m_Header.Layout(&hdlayout);
        m_cyHeader = hdlayout.pwpos->cy;

        // offset from column start to text start
        m_xOffset = bIsComCtl6 ? 9 : 6;

        m_xPos = 0;

        UpdateColumns();

    }

    void UpdateColumns()
    {
        m_cxTotal = 0;

        HDITEM hditem;
        hditem.mask = HDI_WIDTH;
        int nCnt = m_Header.GetItemCount();

        ATLASSERT(nCnt <= m_arrColWidths.size());
        ATLASSERT(nCnt <= m_arrColFormats.size());

        // get column widths from the header control
        for (int i = 0; i < nCnt; i++)
        {
            if (m_Header.GetItem(i, &hditem))
            {
                m_cxTotal += m_arrColWidths[i] = hditem.cxy;
                if (i == 0)
                    m_Tree.m_nFirstColumnWidth = hditem.cxy;
            }
        }
        m_bHeaderChangesBlocked = TRUE;
        RepositionControls();
    }

    void RepositionControls()
    {
        // reposition child controls
        if (m_Tree.m_hWnd)
        {

            CRect rcClient;
            GetClientRect(&rcClient);
            int cx = rcClient.Width();
            int cy = rcClient.Height();

            // get dimensions of scroll bars
            int cyHScroll = GetSystemMetrics(SM_CYHSCROLL);
            int cxVScroll = GetSystemMetrics(SM_CXVSCROLL);

            // determine if vertical scroll bar is visible
            SCROLLINFO scrinfo;
            scrinfo.cbSize = sizeof(scrinfo);
            scrinfo.fMask = SIF_ALL;
            
            m_Tree.GetScrollInfo(SB_VERT, &scrinfo);

            BOOL bVScrollVisible = scrinfo.nMin != scrinfo.nMax ? TRUE : FALSE;

            // determine full header width
            int cxTotal = m_cxTotal + (bVScrollVisible ? cxVScroll : 0);

            if (m_xPos > cxTotal - cx) m_xPos = cxTotal - cx;
            if (m_xPos < 0)	m_xPos = 0;

            scrinfo.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
            scrinfo.nPage = cx;
            scrinfo.nMin = 0;
            scrinfo.nMax = cxTotal;
            scrinfo.nPos = m_xPos;
            m_horScroll.SetScrollInfo(&scrinfo);

            CRect rcTree;
            m_Tree.GetClientRect(&rcTree);

            // move to a negative offset if scrolled horizontally
            int x = 0;
            if (cx < cxTotal)
            {
                x = m_horScroll.GetScrollPos();
                cx += x;
            }

            // show horisontal scroll only if total header width is greater
            // than the client rect width and cleint rect height is big enough
            BOOL bHScrollVisible = rcClient.Width() < cxTotal
                && rcClient.Height() >= cyHScroll + m_cyHeader;

            m_horScroll.ShowWindow(bHScrollVisible ? SW_SHOW : SW_HIDE);

            m_Header.MoveWindow(-x, 0, cx - (bVScrollVisible ? cxVScroll : 0), m_cyHeader);

            m_Header2.MoveWindow(rcClient.Width() - cxVScroll, 0, cxVScroll, m_cyHeader);

            m_Tree.MoveWindow(-x, m_cyHeader, cx, cy - m_cyHeader - (bHScrollVisible ? cyHScroll : 0));

            m_horScroll.MoveWindow(0, rcClient.Height() - cyHScroll,
                rcClient.Width() - (bVScrollVisible ? cxVScroll : 0), cyHScroll);


            // show the second header at the top-right corner 
            // only when vertical scroll bar is visible
            m_Header2.ShowWindow(bVScrollVisible ? SW_SHOW : SW_HIDE);

            RedrawWindow();
        }
    }

    void OnTreeCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        // We use custom drawing to correctly display subitems.
    // Standard drawing code is used only for displaying icons and dotted lines
    // The rest of work is done here.

        NMCUSTOMDRAW* pNMCustomDraw = (NMCUSTOMDRAW*)pNMHDR;
        NMTVCUSTOMDRAW* pNMTVCustomDraw = (NMTVCUSTOMDRAW*)pNMHDR;

        switch (pNMCustomDraw->dwDrawStage)
        {
        case CDDS_PREPAINT:
            *pResult = CDRF_NOTIFYITEMDRAW;
            break;

        case CDDS_ITEMPREPAINT:
            *pResult = CDRF_DODEFAULT | CDRF_NOTIFYPOSTPAINT;
            break;

        case CDDS_ITEMPOSTPAINT:
        {
            HTREEITEM hItem = (HTREEITEM)pNMCustomDraw->dwItemSpec;
            CRect rcItem = pNMCustomDraw->rc;

            if (rcItem.IsRectEmpty())
            {
                // nothing to paint
                *pResult = CDRF_DODEFAULT;
                break;
            }

            CDCHandle dc(pNMCustomDraw->hdc);

            CRect rcLabel;
            m_Tree.GetItemRect(hItem, &rcLabel, TRUE);

            COLORREF crTextBk = pNMTVCustomDraw->clrTextBk;
            COLORREF crWnd = GetSysColor((IsWindowEnabled() ? COLOR_WINDOW : COLOR_BTNFACE));

#ifndef _OWNER_DRAWN_TREE
            // clear the original label rectangle
            dc.FillSolidRect(&rcLabel, crWnd);
#endif //_OWNER_DRAWN_TREE

            int nColsCnt = m_Header.GetItemCount();

            // draw horizontal lines...
            int xOffset = 0;
            for (int i = 0; i < nColsCnt; i++)
            {
                xOffset += m_arrColWidths[i];
                rcItem.right = xOffset - 1;
                dc.DrawEdge(&rcItem, BDR_SUNKENINNER, BF_RIGHT);
            }
            // ...and the vertical ones
            dc.DrawEdge(&rcItem, BDR_SUNKENINNER, BF_BOTTOM);

            WTL::CString strText;
            m_Tree.GetItemText(hItem, strText);
            WTL::CString strSub;
            extractSubString(strSub, strText, 0, '\t');

            // calculate main label's size
            CRect rcText(0, 0, 0, 0);
            dc.DrawText(strSub, strSub.GetLength(),
                rcText, DT_NOPREFIX | DT_CALCRECT);
            rcLabel.right = min(rcLabel.left + rcText.right + 4, m_arrColWidths[0] - 4);

            BOOL bFullRowSelect = m_Tree.GetStyle() & TVS_FULLROWSELECT;

            if (rcLabel.Width() < 0)
                crTextBk = crWnd;
            if (crTextBk != crWnd)	// draw label's background
            {
                CRect rcSelect = rcLabel;
                if (bFullRowSelect) rcSelect.right = rcItem.right;
                dc.FillSolidRect(&rcSelect, crTextBk);

                // draw focus rectangle if necessary
                if (pNMCustomDraw->uItemState & CDIS_FOCUS)
                    dc.DrawFocusRect(&rcSelect);

            }

            // draw main label

            CFontHandle pOldFont;
            if (m_Tree.GetStyle() & TVS_TRACKSELECT && pNMCustomDraw->uItemState && CDIS_HOT)
            {
                LOGFONT lf;
                pOldFont = m_Tree.GetFont();
                pOldFont.GetLogFont(&lf);
                lf.lfUnderline = 1;
                CFont newFont;
                newFont.CreateFontIndirect(&lf);
                dc.SelectFont(newFont);
            }
            dc.SetBkMode(TRANSPARENT);

            rcText = rcLabel;
            rcText.DeflateRect(2, 1);
            dc.SetTextColor(pNMTVCustomDraw->clrText);
            dc.DrawText(strSub, strSub.GetLength(),
                rcText, DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS);
            xOffset = m_arrColWidths[0];

            if (IsWindowEnabled() && !bFullRowSelect)
                dc.SetTextColor(::GetSysColor(COLOR_MENUTEXT));

            // set not underlined text for subitems
            if (pOldFont && !(m_Tree.GetStyle() & TVS_FULLROWSELECT))
                dc.SelectFont(pOldFont);

            // draw other columns text
            for (int i = 1; i < nColsCnt; i++)
            {
                if (extractSubString(strSub, strText, i, '\t'))
                {
                    rcText = rcLabel;
                    rcText.left = xOffset + kColumnMargin;
                    rcText.right = xOffset + m_arrColWidths[i] - kColumnMargin;
                    rcText.DeflateRect(m_xOffset, 1, 2, 1);
                    if (rcText.left < 0 || rcText.right < 0 || rcText.left >= rcText.right)
                    {
                        xOffset += m_arrColWidths[i];
                        continue;
                    }
                    DWORD dwFormat = m_arrColFormats[i] & HDF_RIGHT ?
                        DT_RIGHT : (m_arrColFormats[i] & HDF_CENTER ? DT_CENTER : DT_LEFT);


                    dc.DrawText(strSub, strSub.GetLength(), 
                        rcText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS | dwFormat);
                }
                xOffset += m_arrColWidths[i];
            }

            if (pOldFont) dc.SelectFont(pOldFont);
        }
        *pResult = CDRF_DODEFAULT;
        break;

        default:
            *pResult = CDRF_DODEFAULT;
        }
    }

    void OnHeaderItemChanging(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        // do not allow user to set zero width to the first column.
    // the minimum width is defined by m_uMinFirstColWidth;

        if (m_bHeaderChangesBlocked)
        {
            // do not allow change header size when moving it
            // but do not prevent changes the next time the header will be changed
            m_bHeaderChangesBlocked = FALSE;
            *pResult = TRUE; // prevent changes
            return;
        }

        *pResult = FALSE;

        LPNMHEADER pnm = (LPNMHEADER)pNMHDR;
        if (pnm->iItem == 0)
        {
            CRect rc;
            m_Header.GetItemRect(0, &rc);
            if (pnm->pitem->cxy < m_uMinFirstColWidth)
            {
                // do not allow sizing of the first column 
                pnm->pitem->cxy = m_uMinFirstColWidth + 1;
                *pResult = TRUE; // prevent changes
            }
            return;
        }
    }

    void OnHeaderItemChanged(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        UpdateColumns();

        m_Tree.Invalidate();
    }

protected:
    enum ChildrenIDs { HeaderID = 1, TreeID = 2, HScrollID = 3, Header2ID = 4 };

    CCustomTreeChildCtrl m_Tree;
    CScrollBar m_horScroll;
    CHeaderCtrl m_Header;
    CHeaderCtrl m_Header2;

    int m_cyHeader;
    int m_cxTotal;
    int m_xPos;
    int m_xOffset;
    int m_uMinFirstColWidth;
    BOOL m_bHeaderChangesBlocked;

    std::vector<int> m_arrColWidths;
    std::vector<DWORD> m_arrColFormats;

    static const int kColumnMargin = 1; // 1 pixel between coumn edge and text bounding rectangle

protected:
    LRESULT OnPaint(_In_ UINT /*uMsg*/,
        _In_ WPARAM /*wParam*/,
        _In_ LPARAM /*lParam*/,
        _Out_ BOOL& bHandled) 
    {
        // do not draw entire background to avoid flickering
    // just fill right-bottom rectangle when it is visible

        CPaintDC dc(m_hWnd);

        CRect rcClient;
        GetClientRect(&rcClient);

        int cyHScroll = GetSystemMetrics(SM_CYHSCROLL);
        int cxVScroll = GetSystemMetrics(SM_CXVSCROLL);
        CBrush brush;
        brush.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));

        CRect rc;

        // determine if vertical scroll bar is visible
        SCROLLINFO scrinfo;
        scrinfo.cbSize = sizeof(scrinfo);
        scrinfo.fMask = SIF_ALL;
        m_Tree.GetScrollInfo(SB_VERT, &scrinfo);
        BOOL bVScrollVisible = scrinfo.nMin != scrinfo.nMax ? TRUE : FALSE;

        if (bVScrollVisible)
        {
            // fill the right-bottom rectangle
            rc.SetRect(rcClient.right - cxVScroll, rcClient.bottom - cyHScroll,
                rcClient.right, rcClient.bottom);
            dc.FillRect(rc, brush);
        }

        bHandled = TRUE;
        return 0;
    }

    LRESULT OnEraseBackground(_In_ UINT /*uMsg*/,
        _In_ WPARAM /*wParam*/,
        _In_ LPARAM /*lParam*/,
        _Out_ BOOL& bHandled) 
    {
        bHandled = TRUE;
        return 0;
    }

    LRESULT OnNotify(_In_ UINT /*uMsg*/,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam,
        _Out_ BOOL& bHandled)
    {
        // there are several notifications we need to precess
        auto pHdr = (LPNMHDR)lParam;
        if (pHdr->code == NM_CUSTOMDRAW && pHdr->idFrom == TreeID)
        {
            LRESULT res = 0;
            OnTreeCustomDraw(pHdr, &res);
            bHandled = TRUE; // do not forward
            return res;
        }

        if (pHdr->code == HDN_ITEMCHANGING && pHdr->idFrom == HeaderID)
        {
            LRESULT res = 0;
            OnHeaderItemChanging(pHdr, &res);
            bHandled = TRUE; // do not forward
            return res;
        }

        if (pHdr->code == HDN_ITEMCHANGED && pHdr->idFrom == HeaderID)
        {
            LRESULT res = 0;
            OnHeaderItemChanged(pHdr, &res);
            bHandled = TRUE; // do not forward
            return res;
        }

#ifdef _OWNER_DRAWN_TREE

        if (pHdr->code == TVN_ITEMEXPANDING && pHdr->idFrom == TreeID)
        {
            // avoid bitmap scrolling 
            Invalidate(); // ... and forward
        }

#endif //_OWNER_DRAWN_TREE

        if (pHdr->code == TVN_ITEMEXPANDED && pHdr->idFrom == TreeID)
        {
            RepositionControls(); // ... and forward
        }

        // forward notifications from children to the control owner
        pHdr->hwndFrom = m_hWnd;
        pHdr->idFrom = GetWindowLong(GWL_ID);
        bHandled = (BOOL)GetParent().SendMessage(WM_NOTIFY, wParam, lParam);
        return 0;
    }
    
    void OnEnable(BOOL bEnable) 
    {
        m_Header.EnableWindow(bEnable);
        m_Header2.EnableWindow(bEnable);
        m_Tree.EnableWindow(bEnable);
        m_horScroll.EnableWindow(bEnable);
    }

    void OnSettingChange(UINT uFlags, LPCTSTR lpszSection) 
    {
        m_Tree.SendMessage(WM_SETTINGCHANGE);
        m_horScroll.SendMessage(WM_SETTINGCHANGE);

        // set correct font for the header
        CRect rcClient;
        GetClientRect(&rcClient);

        //CFont* pFont = m_Tree.GetFont();
        //m_Header.SetFont(pFont);
        //m_Header2.SetFont(pFont);

        m_Header.SendMessage(WM_SETTINGCHANGE);
        m_Header2.SendMessage(WM_SETTINGCHANGE);

        m_Header.SetFont((HFONT)GetStockObject(DEFAULT_GUI_FONT));
        m_Header2.SetFont((HFONT)GetStockObject(DEFAULT_GUI_FONT));

        // get header layout
        WINDOWPOS wp;
        HDLAYOUT hdlayout;
        hdlayout.prc = &rcClient;
        hdlayout.pwpos = &wp;
        m_Header.Layout(&hdlayout);
        m_cyHeader = hdlayout.pwpos->cy;

        RepositionControls();
    }

    void OnSize(UINT nType, CSize sz) 
    {
        ATLTRACE("(%d,%d)", sz.cx, sz.cy);
        RepositionControls();
    }

    LRESULT OnHScroll(_In_ UINT msg, _In_ WPARAM wParam, _In_ LPARAM lParam, _Out_ BOOL& bHandled)
    {
        bHandled = TRUE;
        UINT nSBCode = (UINT)LOWORD(wParam);
        UINT nPos = HIWORD(wParam);
        HWND hWndScrollBar = reinterpret_cast<HWND>(lParam);
        SCROLLINFO scrinfo;
        scrinfo.cbSize = sizeof(scrinfo);
        scrinfo.fMask = SIF_ALL;

        m_Tree.GetScrollInfo(SB_VERT, &scrinfo);

        BOOL bVScrollVisible = scrinfo.nMin != scrinfo.nMax ? TRUE : FALSE;

        // determine full header width
        int cxTotal = m_cxTotal + (bVScrollVisible ? GetSystemMetrics(SM_CXVSCROLL) : 0);

        CRect rcClient;
        GetClientRect(&rcClient);
        int cx = rcClient.Width();

        int xLast = m_xPos;

        switch (nSBCode)
        {
        case SB_LINELEFT:
            m_xPos -= 15;
            break;
        case SB_LINERIGHT:
            m_xPos += 15;
            break;
        case SB_PAGELEFT:
            m_xPos -= cx;
            break;
        case SB_PAGERIGHT:
            m_xPos += cx;
            break;
        case SB_LEFT:
            m_xPos = 0;
            break;
        case SB_RIGHT:
            m_xPos = cxTotal - cx;
            break;
        case SB_THUMBTRACK:
            m_xPos = nPos;
            break;
        }

        if (m_xPos < 0)
            m_xPos = 0;
        else if (m_xPos > cxTotal - cx)
            m_xPos = cxTotal - cx;

        if (xLast == m_xPos)
            return 0;

        m_Tree.m_nOffsetX = m_xPos;

        SetScrollPos(SB_HORZ, m_xPos);

        DefWindowProc(msg, wParam, lParam);
        

        RepositionControls();
        return 0;
    }

    void OnCancelMode() 
    {
        m_Header.SendMessage(WM_CANCELMODE);
        m_Header2.SendMessage(WM_CANCELMODE);
        m_Tree.SendMessage(WM_CANCELMODE);
        m_horScroll.SendMessage(WM_CANCELMODE);
    }
};

using CColumnTreeCtrl = CColumnTreeCtrlT<ATL::CWindow>;