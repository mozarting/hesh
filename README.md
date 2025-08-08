# Hesh – A Minimal Shell in C

![Work in Progress](https://img.shields.io/badge/status-WIP-orange) ![License](https://img.shields.io/badge/License-MIT-green)

> 🚧 **Work in Progress**: This project is in development stage.

**hesh** is a lightweight, Unix-like shell written in C from scratch. It supports essential shell features such as command execution, I/O redirection, piping, background processes, and basic built-in commands.

This project was built to deepen understanding of Linux system calls and low-level programming, without relying on external libraries or frameworks.

---

## Features

- [x] Minimal, clean REPL-style prompt
- [x] Execute system commands (e.g., `ls`, `grep`, `cat`)
- [x] Built-in commands: `cd`, `pwd`, `exit`
- [ ] Input/output redirection (`>`, `<`)
- [ ] Command piping (`|`)
- [ ] Background process support (`&`)

---

## Build & Run

### Requirements:

- GCC or any C compiler
- POSIX-compliant system (Linux/macOS)

### Build:

```bash
gcc -o hesh shell.c
```

### Run:

```bash
./hesh
```

---

## System Calls Used

- `fork()`, `execvp()`, `waitpid()`
- `dup2()`, `pipe()`
- `open()`, `close()`
- `chdir()`, `getcwd()`
- `signal()`

---

## Project Structure

```
shell.c       # Main shell implementation
README.md     # You're here
```

---

## Limitations

- No advanced features like autocomplete or scripting
- No support for quotes, escaping, or complex parsing (yet)

---

## Roadmap (maybe someday...)

- [ ] Command history (`↑ ↓`)
- [ ] Tab completion
- [ ] Config file support
- [ ] Job control (bg/fg/kill)

---

## Sample Commands

```bash
ls -l
cat file.txt > out.txt
grep 'error' < log.txt | wc -l
sleep 10 &
cd /tmp
exit
```

---

## License

MIT License

---

## Author

Mahesh Odedara
