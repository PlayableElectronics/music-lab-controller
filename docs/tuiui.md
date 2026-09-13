# tuiui investigation

The project at [github.com/jaylfc/tuiui](https://github.com/jaylfc/tuiui) is a
Rust terminal desktop/window-manager experiment. Its source pulls in a full
terminal-rendering stack (including `alacritty_terminal`) and its published
binary support is aimed at desktop macOS/Linux architectures, not the
CV1800B's riscv64 Buildroot environment.

It would therefore require a Rust riscv64-musl toolchain and a measured
cross-build before it could be considered. That is not appropriate for the
64 MB first milestone: the dependency and runtime footprint is substantially
larger than the current shell TUI, and no compatible binary was observed on
the Duo. The zero-dependency `tui/lab-tui` remains the primary frontend.

If this is revisited, measure the stripped binary, peak RSS, startup time, and
the effect of its terminal dependencies on the Duo before installing it.

The official Milk-V guidance distinguishes the original 64 MB Duo from the
larger Duo variants, so any future tuiui experiment must be measured against
the original board's memory budget. It must not become a prerequisite for
controller operation.
