import argparse
from outliers_filter_mode import OutliersFilterMode


class OutlierParseAction(argparse.Action):

    def __call__(self, parser, namespace, values, option_string=None):
        if values == 1:
            outlier_type = OutliersFilterMode.PERCENTAGE
        elif values == 2:
            outlier_type = OutliersFilterMode.MODE
        elif values == 3:
            outlier_type = OutliersFilterMode.FILTER
        else:
            raise ValueError('Illegal Outlier filter mode')
        setattr(namespace, self.dest, outlier_type)


class ArgParser:
    def __init__(self):
        self.parser = argparse.ArgumentParser(description='Analyze the client RTT times')
        # Data File parsing
        self.parser.add_argument('filename', metavar='filename', type=argparse.FileType('r'),
                                 help='The file name containing raw output data')
        self.parser.add_argument('title', metavar='title', type=str, help='The title of the plot')
        # Top level arguments for specific plots
        self.parser.add_argument('--show-rtts', action='store_true', help='Show the rtt plot')
        self.parser.add_argument('--show-histogram', action='store_true', help='Show the rtt histogram')
        self.parser.add_argument('--cr-detection', action='store_true', help='Calculate the CR detection score')
        self.parser.add_argument('--ks_test', action='store_true', help='Calculate the KS test score')
        self.parser.add_argument('--show-histogram-control', action='store_true',
                                 help='Show the rtt histogram with control sequence')

        # Group level arguments for individual plots
        outlier_matching = self.parser.add_argument_group('outlier_matching',
                                                          'Arguments to filter outliers in the data')
        outlier_matching.add_argument('--remove-outliers', action='store_true',
                                      help='Remove outliers from the data')
        outlier_matching.add_argument('--outlier-removal-type', type=int, action=OutlierParseAction, choices=[1, 2, 3])

        histogram_options = self.parser.add_argument_group('histogram_options',
                                                           'Arguments to modify the histogram chart')
        histogram_options.add_argument('--full-histogram', action='store_true',
                                       help='Show histogram for all RTT or separated by whether the flooder was running')
        self.args = self.parser.parse_args()
        if self.args.remove_outliers and self.args.outlier_removal_type is None:
            self.parser.error('Remove outliers requires the removal type')

    @property
    def show_rtts(self):
        return self.args.show_rtts

    @property
    def show_histogram(self):
        return self.args.show_histogram

    @property
    def cr_detection(self):
        return self.args.cr_detection

    @property
    def show_control_histogram(self):
        return self.args.show_histogram_control

    @property
    def ks_test(self):
        return self.args.ks_test

    @property
    def full_histogram(self):
        return self.args.full_histogram

    @property
    def file(self):
        return self.args.filename

    @property
    def remove_outliers(self):
        return self.args.remove_outliers

    @property
    def outlier_removal_type(self):
        return self.args.outlier_removal_type

    @property
    def title(self):
        return self.args.title
