target remote :1234
directory src/
set disassemble-next-line on
info break
set pagination off
set logging file gdb.log
set logging on
break main
continue
set $i = 0
while ( $i < 5000)
  info registers
  step
  # next
  # continue
  set $i = $i + 1
end
where
