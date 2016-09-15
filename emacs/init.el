(global-linum-mode t)
(setq-default c-default-style "k&r")
(setq-default indent-tabs-mode t)
(setq-default tab-width 4)
(setq-default c-basic-offset 4)
(setq-default c-backspace-function 'backward-delete-char)
(setq-default show-trailing-whitespace t)
(defalias 'hpr 'haskell-process-restart)
(menu-bar-mode -1)
(tool-bar-mode -1)
(scroll-bar-mode -1)

(defun my-fullscreen ()
  (interactive)
  (set-frame-parameter nil 'fullscreen 'fullboth))

(defun my-non-fullscreen ()
  (interactive)
  (set-frame-parameter nil 'width 82)
  (set-frame-parameter nil 'fullscreen 'fullheight))

(defun toggle-fullscreen ()
  (interactive)
  (if (eq (frame-parameter nil 'fullscreen) 'fullboth)  ;tests if already fullscreened
      (my-non-fullscreen)
    (my-fullscreen)))

(global-set-key (kbd "<f11>") 'toggle-fullscreen) 

(custom-set-variables
 ;; custom-set-variables was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(custom-safe-themes
   (quote
	("a9d3f08b198f56b8f20b73027e049da9b6f3344a141ef62007776037d47d6d08" "c5cecf52d9ef8ce610c2ed2cbbed477061f8445216a97d05218d61e6e795f4fa" "72ac74b21322d3b51235f3b709c43c0721012e493ea844a358c7cd4d57857f1f" default)))
 '(package-archives
   (quote
	(("gnu" . "http://elpa.gnu.org/packages/")
	 ("melpa-stable" . "http://stable.melpa.org/packages/"))))
 '(package-selected-packages (quote (haskell-mode)))
 '(tex-fontify-script nil))
(custom-set-faces
 ;; custom-set-faces was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(subscript ((nil nil)))
 '(superscript ((nil nil))))

(load-theme 'seti)

;; Added by Package.el.  This must come before configurations of
;; installed packages.  Don't delete this line.  If you don't want it,
;; just comment it out by adding a semicolon to the start of the line.
;; You may delete these explanatory comments.
(package-initialize)

(require 'package)

