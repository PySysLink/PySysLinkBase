Reports generated using codechecker. To see the reports, run:

```console
firefox index.html
```

To generate the records, navigate to the folder .codechecker, and run:

```console
cmake -DENABLE_TESTS=ON ..
```

Which generates the file compile_commands.json. Then run:

```console
CodeChecker analyze compile_commands.json -o reports
```

To generate the reports on the reports/ folder.

To analyze the reports on console, run:

```console
CodeChecker parse reports
```

To generate HTML files, run:

```console
CodeChecker parse --export html --output ./reports_html ./reports
```