#!/usr/bin/perl -w

use Gtk;
use strict 'vars';

init Gtk;

my ($main_window,$table);
my @results;

sub item_factory_cb 
{
  my ($widget, $action, @data) = @_;

  if($action == -1)
  {
    destroy $main_window; 
    exit
  }
}

my @item_factory_entries = ();

sub create_item_factory 
{
  my ($accel_group, $item_factory, $box1, $label, $box2);
  my ($separator, $button, $dummy);
    
  $main_window = new Gtk::Window('toplevel');
  $accel_group = new Gtk::AccelGroup;
  $item_factory = new Gtk::ItemFactory('Gtk::MenuBar', "<main>", 
				       $accel_group);
  
#  my @chips = `findchip -l`;
#  push @item_factory_entries, ["/_File",	undef,	0,	"<Branch>"];
#  foreach (@chips)
#  {
#    chomp;
#    my $item = "/_File/Probe ".$_;
#    push @item_factory_entries, [$item, undef,	1];
#  }
#  push @item_factory_entries, ["/File/_All", "<control>A",	0];
  push @item_factory_entries, ["/File/_Quit",	"<control>Q",	-1];
  
  $accel_group->attach($main_window);
  foreach (@item_factory_entries) {
    $item_factory->create_item($_, \&item_factory_cb);
  }
  
  $main_window->set_title("Linux IrDA Chip Probe");
  $main_window->set_border_width(0);
  
  $box1 = new Gtk::VBox(0, 0);
  $main_window->add($box1);
  $box1->pack_start($item_factory->get_widget('<main>'), 0, 0, 0);
  
  $table = new Gtk::Table(3,6,1);
  my $lab00 = new Gtk::Label "Chip:";
  my $lab01 = new Gtk::Label "Model:";
  my $lab02 = new Gtk::Label "Version:";
  my $lab10 = new Gtk::Label "Control IO:";
  my $lab11 = new Gtk::Label "SIR IO:";
  my $lab12 = new Gtk::Label "FIR IO:";
  my $lab20 = new Gtk::Label "IRQ:";
  my $lab21 = new Gtk::Label "DMA:";
  my $lab22 = new Gtk::Label "";

  for(my $i = 0; $i < 9; $i++)
  {
    $results[$i] = new Gtk::Label "";
    my ($x,$y);
    $x = 2*($i % 3);
    $y = $i / 3;
    $table->attach_defaults($results[$i], $x+1,$x+2, $y,$y+1);
  }

  $table->set_col_spacings(6);
  $table->set_row_spacings(4);

  $table->attach_defaults($lab00, 0,1,0,1);
  $table->attach_defaults($lab01, 2,3,0,1);
  $table->attach_defaults($lab02, 4,5,0,1);
  $table->attach_defaults($lab10, 0,1,1,2);
  $table->attach_defaults($lab11, 2,3,1,2);
  $table->attach_defaults($lab12, 4,5,1,2);
  $table->attach_defaults($lab20, 0,1,2,3);
  $table->attach_defaults($lab21, 2,3,2,3);
  $table->attach_defaults($lab22, 4,5,2,3);
  my ($res);
  $res = `findchip`;
  chomp $res;
  my @chipinfo = split (/,/,$res);
  
  for(my $i = 0; $i < 8; $i++)
  {
    if(defined $chipinfo[$i])
    {
      $results[$i]->set ($chipinfo[$i]);
    }
    else
    {
      $results[$i]->set ("?");
    }
  }

  $box1->add($table);
  
}

create_item_factory;
$main_window->signal_connect("destroy" => sub { destroy $main_window; exit } );
show_all $main_window;

main Gtk;
