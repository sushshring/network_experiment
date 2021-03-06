#!/usr/bin/env python3.7
from analyzer import Analyzer
from parser.arg_parser import ArgParser
from parser.data_parser import Parser


def main():
    arg_parser = ArgParser()
    parser = Parser(arg_parser.file)
    analyzer = Analyzer(parser.rtts, parser.rtts_control)
    analyzer.analyze(arg_parser)


if __name__ == '__main__':
    main()
