#!/usr/bin/perl -w
use strict;
use XML::LibXML;
use feature qw/state/;

sub GenerateRandomString 
{
	return undef if ( @_ != 3 );
	my ( $min_len, $max_len, $alpha ) = @_;
	my $len = int rand( $max_len - $min_len + 1 ) + $min_len;
	my $result = "";
	my $alpha_size = length( $alpha );
	for ( my $i = 0; $i < $len; $i++ )
	{
		my $index = int rand( $alpha_size );
		$result .= substr( $alpha, $index, 1 );
	}
	return $result;
}

sub GenerateRandomKey
{
	state %entityKeys;
	my $key = undef;
	do
	{
		$key = GenerateRandomString( 16, 16, "0123456789abcdef" );
	}
	while ( exists $entityKeys{ $key } );
	$entityKeys{ $key } = 1;
	return $key;
}


# ==== Entry Point ====
die "Usage: ./keys-to-struc.pl <keys-path>" if ( @ARGV < 1 );

my $keysPath = $ARGV[ 0 ];
open my $keysFile, "<$keysPath" or die "Can't open input file\n";
my @allLines = <$keysFile>;
close $keysFile;

my $recordInProcess = 0;
my %record = ();
my $xmlDoc = new XML::LibXML::Document( "1.0", "utf-8" );

my $rootNode = $xmlDoc->createElement( "root" );

my $treeNode = $xmlDoc->createElement( "tree" );
my $keysNode = $xmlDoc->createElement( "keyents" );

$rootNode->appendChild( $xmlDoc->createElement( "header" ) );
$rootNode->appendChild( $treeNode );
$rootNode->appendChild( $keysNode );

my @tree = ( {}, $treeNode );
for my $line ( @allLines )
{
	chomp $line;
	if ( $line =~ /^Path:/ )
	{
		$recordInProcess = 1;
	}
	elsif ( $line eq "========" )
	{
		my $entKey = GenerateRandomKey();
		
		my $path = $record{ "Path" };
		my @pathParts = split( "/", $path );
		shift @pathParts;
		my $dirsCount = @pathParts - 1;
		my $curFolderNode = $treeNode;
		my $folderNode = undef;
		my $curTree = \@tree;
		my $partIndex = 0;
		
		for ( ; $partIndex < $dirsCount; $partIndex++ )
		{
			my $curName = $pathParts[ $partIndex ];
			if ( exists $curTree->[ 0 ]->{ $curName } )
			{
				$curTree = $curTree->[ 0 ]->{ $curName };
				$curFolderNode = $curTree->[ 1 ];
			}
			else
			{
				$folderNode = $xmlDoc->createElement( "node" );
				$folderNode->setAttribute( "name", $curName );
				$curFolderNode->appendChild( $folderNode );
				$curFolderNode = $folderNode;
				$curTree->[ 0 ]->{ $curName } = [ {}, $folderNode ];
				$curTree = $curTree->[ 0 ]{ $curName };
			}
		}
		
		if ( $dirsCount >= 0 )
		{
			my $lastPart = $pathParts[ $partIndex ];
			my $keyNode = $xmlDoc->createElement( "node" );
			$keyNode->setAttribute( "name", $lastPart );
			$keyNode->setAttribute( "key", $entKey );
			$curFolderNode->appendChild( $keyNode );
		}
		
		my $newKeyNode = $xmlDoc->createElement( "ent" );
		$newKeyNode->setAttribute( "key", $entKey );
		if ( not exists( $record{ "login" } ) )
		{
			$record{ "login" } = "";
		}
		for my $recordKey ( keys %record )
		{
			next if ( $recordKey eq "Path" );
			next if ( $recordKey eq "title" );
			my $recordField = $xmlDoc->createElement( $recordKey );
			$recordField->appendTextNode( $record{ $recordKey } );
			$newKeyNode->appendChild( $recordField );
		}
		$keysNode->appendChild( $newKeyNode );
		
		$recordInProcess = 0;
		%record = ();
	}
	
	if ( $recordInProcess )
	{
		my ( $key, $value ) = $line =~ /^(.+): \[(.+)\]$/;
		if ( not defined $key )
		{
			die "no defined! [$line]";
		}
		else {
			$record{ $key } = $value;
		}
	}
}

$xmlDoc->setDocumentElement( $rootNode );
print $xmlDoc->toString( 1 );
