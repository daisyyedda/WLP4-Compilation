#lang racket

(require "scanning.rkt")

(for [(line (in-lines))]
  (displayln (scan line)))