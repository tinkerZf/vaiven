#!/bin/bash

cat <<EOF
fn loop of x is
  for x < 50000000 do
    x = x + 1;
  end
end

var primings = 10;
for primings > 0 do
  loop(50000000);
  primings = primings - 1;
end

loop(1);

EOF
